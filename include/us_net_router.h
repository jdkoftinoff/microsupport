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
#ifndef US_NET_ROUTER_H
#define US_NET_ROUTER_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif
#include "us_net.h"
#include "us_time.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup us_net_router
 */
/*@{*/

#ifndef US_NET_ROUTER_MAX_ROUTES
/// The maximum number of unique non-multicast addresses we can keep track of
#define US_NET_ROUTER_MAX_ROUTES (1024)
#endif

#ifndef US_NET_ROUTER_ROUTE_EXPIRY_TIME_IN_SECONDS
/// The time in seconds that we flush inactive addresses from the routing table
#define US_NET_ROUTER_ROUTE_EXPIRY_TIME_IN_SECONDS (30)
#endif

#ifndef US_NET_ROUTER_ROUTE_CLEANUP_RATE_IN_SECONDS
/// the time in seconds that we wait before trying to cleanup the routing table
#define US_NET_ROUTER_ROUTE_CLEANUP_RATE_IN_SECONDS (120)
#endif

#ifndef US_NET_ROUTER_ROUTE_FLAP_RATE_IN_SECONDS
/// The time in seconds that we wait after a route changes before changing it again
#define US_NET_ROUTER_ROUTE_FLAP_RATE_IN_SECONDS (4)
#endif


/// Generic address format agnostic network address map/router item
typedef struct us_net_router_item_s {
    /// The context of the item
    void *context;
    /// The generic tag value of the item
    int tag;
    /// The address of the item
    struct sockaddr_storage address;
    /// The length of the address. 0 means not active.
    socklen_t address_length;
    /// The time it was last seen
    time_t last_seen_time;
    /// The time it was last changed
    time_t last_changed_time;
} us_net_router_item_t;


/// Initialize a net_router_item
void us_net_router_item_init(
        us_net_router_item_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time );

/// Compare two router items
int us_net_router_item_compare(
        void const *lhs_,
        void const *rhs_ );


/// Generic address format agnostic network address map/router item
typedef struct us_net_router_s {
    // the last time we did a cleanup
    time_t last_cleanup_time;
    // the count of the items in the routing table
    int routing_table_count;
    // The routing table
    us_net_router_item_t routing_table[US_NET_ROUTER_MAX_ROUTES];
} us_net_router_t;

void us_net_router_init( us_net_router_t *self );

void us_net_router_tick( us_net_router_t *self, time_t cur_time );

void us_net_router_add(
        us_net_router_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time );

void us_net_router_update_or_add(
        us_net_router_t *self,
        void *context,
        int tag,
        struct sockaddr *addr,
        socklen_t addrlen,
        time_t cur_time );

us_net_router_item_t * us_net_router_find(
        us_net_router_t *self,
        struct sockaddr *addr,
        socklen_t addrlen );

void us_net_router_cleanup(
        us_net_router_t *self,
        time_t cur_time );
/*@}*/

#ifdef __cplusplus
}
#endif

#endif

