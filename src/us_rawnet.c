#if defined(__linux__) 

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <linux/sockios.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/types.h>
#include <string.h>       
#include <errno.h>

#include "us_world.h"
#include "us_rawnet.h"
#include "us_logger.h"

/*
Copyright (c) 2010, Meyer Sound Laboratories, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

int us_rawnet_socket( uint16_t ethertype, uint8_t my_mac[6], int *interface_id, const char *interface_name )
{
    int fd=socket(AF_PACKET,SOCK_RAW,htons(ethertype) );
    if( fd>=0 && interface_name ) 
    {
        int i;
        struct ifreq ifr;
        strncpy(ifr.ifr_name,interface_name, sizeof(ifr.ifr_name)-1 );
        if ( ioctl(fd ,SIOCGIFINDEX,&ifr)<0 )
        {
            close( fd );
            return -1;
        }
        if( interface_id )
        {
            *interface_id = ifr.ifr_ifindex;
        }
        if ( ioctl(fd ,SIOCGIFHWADDR,&ifr)<0 )
        {
            close(fd);
            return -1;
        }
        if( my_mac )
        {
            for ( i=0; i<6; ++i )
            {
                my_mac[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
            }
        }
    }
    return fd;
}


int us_rawnet_send( int fd, int interface_id, uint8_t src_mac[6], uint8_t dest_mac[6], uint16_t ethertype, const void *payload, int payload_len )
{
    struct sockaddr_ll socket_address;
    uint8_t buffer[ETH_FRAME_LEN];
    unsigned char *etherhead = buffer;
    unsigned char* data = buffer + 14;
    struct ethhdr *eh = (struct ethhdr *)etherhead;
    socket_address.sll_family   = PF_PACKET;
    socket_address.sll_protocol = htons(ethertype);
    socket_address.sll_ifindex  = interface_id;
    socket_address.sll_hatype   = ARPHRD_ETHER;
    socket_address.sll_pkttype  = PACKET_OTHERHOST;
    socket_address.sll_halen    = ETH_ALEN;
    memcpy(socket_address.sll_addr,src_mac,ETH_ALEN );
    socket_address.sll_addr[6]  = 0x00;
    socket_address.sll_addr[7]  = 0x00;
    memcpy((void*)buffer, (void*)dest_mac, ETH_ALEN);
    memcpy((void*)(buffer+ETH_ALEN), (void*)src_mac, ETH_ALEN);
    eh->h_proto = htons(ethertype);
    memcpy( data, payload, payload_len );
    return sendto(fd, buffer, payload_len+14, 0,
                  (struct sockaddr*)&socket_address, sizeof(socket_address));
}


#endif

