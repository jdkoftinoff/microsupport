#ifndef US_REACTOR_HANDLER_UDP_H
#define US_REACTOR_HANDLER_UDP_H

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
#include "us_world.h"
#include "us_reactor.h"
#include "us_net.h"
#include "us_packet_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \ingroup poll_reactor */
/*@{*/

struct us_reactor_handler_udp_s;

/** \addtogroup reactor_handler_udp reactor_handler_udp
*/
/*@{*/
typedef struct us_reactor_handler_udp_s
{
    us_reactor_handler_t m_base;
    us_packet_queue_t *m_incoming_packets;
    us_packet_queue_t *m_outgoing_packets;
    bool ( *tick )( struct us_reactor_handler_udp_s *self );
    bool ( *queue_readable )( struct us_reactor_handler_udp_s *self );
    bool ( *packet_received )( struct us_reactor_handler_udp_s *self,
                               const us_packet_t *packet,
                               us_packet_queue_t *outgoing_queue );
} us_reactor_handler_udp_t;

/**
*/
us_reactor_handler_t *us_reactor_handler_udp_create( us_allocator_t *allocator );

/**
*/
bool us_reactor_handler_udp_init_ex( us_reactor_handler_t *self,
                                     us_allocator_t *allocator,
                                     void *extra,
                                     const char *ethernet_port,
                                     struct addrinfo *localaddr,
                                     struct addrinfo *multicastaddr,
                                     size_t input_packets,
                                     size_t output_packets,
                                     size_t max_packet_size );

bool us_reactor_handler_udp_init(
    us_reactor_handler_t *self, us_allocator_t *allocator, int fd, void *extra, size_t max_udp_packet_size );

/**
*/
void us_reactor_handler_udp_destroy( us_reactor_handler_t *self );

/**
  */
void us_reactor_handler_udp_close( us_reactor_handler_t *self );

/**
 */
bool us_reactor_handler_udp_tick( us_reactor_handler_t *self );

/**
*/
bool us_reactor_handler_udp_readable( us_reactor_handler_t *self );

/**
*/
bool us_reactor_handler_udp_writable( us_reactor_handler_t *self );

/**
  */
bool us_reactor_handler_udp_queue_readable( us_reactor_handler_udp_t *self );

/*@}*/
/*@}*/

#ifdef __cplusplus
}
#endif

#endif
