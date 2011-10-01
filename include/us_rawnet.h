#ifndef US_RAWNET_H
#define US_RAWNET_H

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

#ifndef US_WORLD_H
#include "us_world.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif


    /** \addtogroup us_rawnet
     */
    /*@{*/
#if defined(__linux__)

    int us_rawnet_socket( uint16_t ethertype, uint8_t my_mac[6], int *interface_id, const char *interface_name );

    int us_rawnet_send( int fd, int interface_id, uint8_t src_mac[6], uint8_t dest_mac[6], uint16_t ethertype, void *payload, int payload_len );

    int us_rawnet_send_vlan( int fd, uint8_t src_mac[6], uint8_t dest_mac[6], uint32_t vlanbits,uint16_t ethertype, void *payload, int payload_len );


#endif

#ifdef __cplusplus
}
#endif

#endif
