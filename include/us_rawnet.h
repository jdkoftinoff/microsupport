#ifndef US_RAWNET_H
#define US_RAWNET_H

/*
 Copyright (c) 2012, J.D. Koftinoff Software, Ltd.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef US_WORLD_H
#include "us_world.h"
#endif


#if US_ENABLE_RAW_ETHERNET==1
# if defined(__APPLE__)
#  include <net/if_dl.h>  
# elif defined(__linux__)
#  include <linux/if_packet.h>
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup us_rawnet
 */
/*@{*/

#ifdef __linux__
# define US_AF_LINK AF_PACKET
# define US_PF_LINK PF_PACKET
typedef struct sockaddr_ll us_sockaddr_dl;
static inline void us_sockaddr_dl_set_mac( struct sockaddr *addr, uint8_t const mac[6] ) {
	us_sockaddr_dl *dl = (us_sockaddr_dl *)addr;
    dl->sll_family=US_AF_LINK; 
	dl->sll_protocol=0; 
	dl->sll_ifindex=0; 
	dl->sll_hatype=0; 
	dl->sll_pkttype=0; 
	dl->sll_halen=6; 
	memcpy( dl->sll_addr, mac, 6 ); 
}
static inline uint8_t const *us_sockaddr_dl_get_mac( struct sockaddr const *addr ) {
	us_sockaddr_dl const *dl = (us_sockaddr_dl const *)addr;
	return dl->sll_addr;
}
#elif defined(__APPLE__) 
# define US_AF_LINK AF_LINK
# define US_PF_LINK PF_LINK
  typedef struct sockaddr_dl us_sockaddr_dl;
static inline void us_sockaddr_dl_set_mac( struct sockaddr *addr, uint8_t const mac[6] ) {
	us_sockaddr_dl *dl = (us_sockaddr_dl *)addr;
	dl->sdl_len=sizeof(us_sockaddr_dl);
	dl->sdl_family=US_AF_LINK;
	dl->sdl_index=0; 
	dl->sdl_type=0; 
	dl->sdl_alen=6; 
	dl->sdl_nlen=0; 
	dl->sdl_slen=0; 
	memcpy( dl->sdl_addr + dl->sdl_nlen, mac, 6 ); 
}
static inline uint8_t const *us_sockaddr_dl_get_mac( struct sockaddr const *addr ) {
	us_sockaddr_dl const *dl = (us_sockaddr_dl const *)addr;
	return dl->sdl_addr + dl->sdl_nlen;
}
#else
# define US_AF_LINK AF_LINK
# define US_PF_LINK PF_LINK
  typedef struct sockaddr_dl us_sockaddr_dl;
static inline void us_sockaddr_dl_set_mac( struct sockaddr *addr, uint8_t const mac[6] ) {
	us_sockaddr_dl *dl = (us_sockaddr_dl *)addr;
	dl->sdl_len=sizeof(us_sockaddr_dl);
	dl->sdl_family=US_AF_LINK;
	dl->sdl_index=0; 
	dl->sdl_type=0; 
	dl->sdl_alen=6; 
	dl->sdl_nlen=0; 
	dl->sdl_slen=0; 
	memcpy( dl->sdl_addr + dl->sdl_nlen, mac, 6 ); 
}
static inline uint8_t const *us_sockaddr_dl_get_mac( struct sockaddr const *addr ) {
	us_sockaddr_dl const *dl = (us_sockaddr_dl const *)addr;
	return dl->sdl_addr + dl->sdl_nlen;
}
#endif

typedef struct us_rawnet_context_s {
    int m_fd;
    uint16_t m_ethertype;
    uint8_t m_my_mac[6];
    uint8_t m_default_dest_mac[6];
    int m_interface_id;
    void *m_additional;
#if defined(US_ENABLE_PCAP)
    void *m_pcap;
#endif
} us_rawnet_context_t;

int us_rawnet_socket(us_rawnet_context_t *self,
                     uint16_t ethertype,
                     const char *interface_name,
                     const uint8_t join_multicast[6]);

void us_rawnet_close(us_rawnet_context_t *self);

ssize_t us_rawnet_send(us_rawnet_context_t *self, const uint8_t dest_mac[6], const void *payload, ssize_t payload_len);

ssize_t us_rawnet_recv(
    us_rawnet_context_t *self, uint8_t src_mac[6], uint8_t dest_mac[6], void *payload_buf, ssize_t payload_buf_max_size);

bool us_rawnet_join_multicast(us_rawnet_context_t *self, const uint8_t multicast_mac[6]);

#ifdef __cplusplus
}
#endif

#endif
