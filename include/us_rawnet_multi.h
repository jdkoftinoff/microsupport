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
#ifndef US_RAWNET_MULTI_H
#define US_RAWNET_MULTI_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif
#include "us_rawnet.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup us_rawnet
 */
/*@{*/

#if defined(__APPLE__)
#include <ifaddrs.h>
#include <net/if_types.h>
#endif

#if defined(__linux__)
#include <ifaddrs.h>
#endif


#ifndef US_RAWNET_MULTI_MAX
#define US_RAWNET_MULTI_MAX (16)
#endif

typedef struct us_rawnet_multi_s {
    us_rawnet_context_t ethernet_ports[US_RAWNET_MULTI_MAX];
    int ethernet_port_count;
} us_rawnet_multi_t;

bool us_rawnet_multi_open(
        us_rawnet_multi_t *self,
        uint16_t ethertype,
        uint8_t const *multicast_address1,
        uint8_t const *multicast_address2);

void us_rawnet_multi_close(us_rawnet_multi_t *self);

int us_rawnet_multi_receive(
    us_rawnet_multi_t *self,
    void *context,
    bool (*handler)(us_rawnet_multi_t *self, void *context, uint8_t *buf, uint16_t len )
    );

int us_rawnet_multi_send(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     );

int us_rawnet_multi_send_reply(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     );

void us_rawnet_multi_rawnet_poll_incoming(
        us_rawnet_multi_t *self,
        int max_poll_count,
        void *context,
        bool (*handler)( us_rawnet_multi_t *self, void *context, uint8_t *buf, uint16_t len ) );


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
