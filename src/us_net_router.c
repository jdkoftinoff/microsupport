/*
 Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
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
#include "us_net_router.h"
#if defined(US_ENABLE_RAW_ETHERNET)
#include "us_rawnet.h"
#endif

void us_net_router_item_init(
        us_net_router_item_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time ) {
    self->context = context;
    self->tag = tag;

    memcpy( &self->address, addr, addrlen );
    self->address_length = addrlen;
    self->last_seen_time = cur_time;
    self->last_changed_time = cur_time;
}

int us_net_router_item_compare(
        void const *lhs_,
        void const *rhs_ ) {

    int cmp=0;
    us_net_router_item_t const *lhs = (us_net_router_item_t const *)lhs_;
    us_net_router_item_t const *rhs = (us_net_router_item_t const *)rhs_;

    // when the item is not active, push it to the end so it can be garbage collected.
    if( lhs->address_length == 0 ) {
        cmp=1;
    } else if( rhs->address_length==0 ) {
        cmp=-1;
    } else {
        // both are active, so then we compare the actual address
        // First compare address length.
        if( lhs->address_length > rhs->address_length ) {
            cmp=1;
        } else if( lhs->address_length < rhs->address_length ) {
            cmp=-1;
        } else {
            // The address_length is the same. Compare address family
            if( lhs->address.ss_family > rhs->address.ss_family ) {
                cmp=1;
            } else if( lhs->address.ss_family < rhs->address_length ) {
                cmp=-1;
            } else {
                // The ss_family is the same. Now compare the actual destination address.
                switch( lhs->address.ss_family ) {
                    case AF_INET:
                        {
                            // compare just the ipv4 address portion
                            struct sockaddr_in const *lhsin = (struct sockaddr_in const *)lhs;
                            struct sockaddr_in const *rhsin = (struct sockaddr_in const *)rhs;
                            cmp = memcmp(&lhsin->sin_addr,&rhsin->sin_addr,sizeof(lhsin->sin_addr));
                        }
                        break;
                    case AF_INET6:
                        {
                            // compare just the ipv6 address portion
                            struct sockaddr_in6 const *lhsin6 = (struct sockaddr_in6 const *)lhs;
                            struct sockaddr_in6 const *rhsin6 = (struct sockaddr_in6 const *)rhs;
                            cmp = memcmp(&lhsin6->sin6_addr,&rhsin6->sin6_addr,sizeof(lhsin6->sin6_addr));
                        }
                        break;
#if defined(US_ENABLE_RAW_ETHERNET)
                    case US_AF_LINK:
                        {
                            // Use portability function to extract ethernet mac address portably
                            uint8_t const *lhsmac = us_sockaddr_dl_get_mac((struct sockaddr const *)&lhs->address);
                            uint8_t const *rhsmac = us_sockaddr_dl_get_mac((struct sockaddr const *)&rhs->address);
                            cmp = memcmp(lhsmac,rhsmac,6);
                        }
                        break;
#endif
                    default:
                        // unrecognized address format, just memcmp the whole thing to allow stable sort
                        cmp = memcmp(&lhs->address,&rhs->address,lhs->address_length);
                        break;
                }
            }
        }
    }
    return cmp;
}


void us_net_router_init( us_net_router_t *self ) {
    // TODO

}

void us_net_router_tick( us_net_router_t *self, time_t cur_time ) {
    // TODO

}

void us_net_router_add(
        us_net_router_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time ) {
    // TODO

}

void us_net_router_update_or_add(
        us_net_router_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time ) {
    // TODO

}

us_net_router_item_t * us_net_router_find(
        us_net_router_t *self,
        struct sockaddr *addr,
        socklen_t addrlen ) {
    // TODO
    return 0;
}

void us_net_router_cleanup(
        us_net_router_t *self,
        time_t cur_time ) {
    // TODO

}

