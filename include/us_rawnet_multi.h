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
#include "us_net.h"
#include "us_rawnet.h"
#include "us_time.h"


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
/// The maximum number of ethernet ports we can manage simultaneously
#define US_RAWNET_MULTI_MAX (16)
#endif

#ifndef US_RAWNET_MULTI_MAC_TABLE_SIZE
/// The maximum number of unique non-multicast MAC addresses we can keep track of
#define US_RAWNET_MULTI_MAC_TABLE_SIZE (1024)
#endif

#ifndef US_RAWNET_MULTI_MAC_ROUTE_EXPIRY_TIME_IN_SECONDS
/// The time in seconds that we flush inactive mac addresses from the routing table
#define US_RAWNET_MULTI_MAC_ROUTE_EXPIRY_TIME_IN_SECONDS (30)
#endif

#ifndef US_RAWNET_MULTI_ROUTE_CLEANUP_RATE_IN_SECONDS
/// The time in seconds that we wait before trying to cleanup the routing table
#define US_RAWNET_MULTI_ROUTE_CLEANUP_RATE_IN_SECONDS (120)
#endif

#ifndef US_RAWNET_MULTI_ROUTE_FLAP_RATE_IN_SECONDS
/// The time in seconds that we wait after a route changes before changing it again
#define US_RAWNET_MULTI_ROUTE_FLAP_RATE_IN_SECONDS (4)
#endif


/// The information for one known MAC address, which interface it was seen on, and when the last time was that it was seen
typedef struct us_rawnet_multi_mac_item_s {
    /// The interface it was seen on, or -1 if not
    int last_seen_interface;
    /// The MAC address
    uint8_t mac[6];
    /// The time it was seen
    time_t last_seen_time;
    /// The time it was last changed
    time_t last_changed_time;
} us_rawnet_multi_mac_item_t;

/// Manage multiple link layer ethernet ports
typedef struct us_rawnet_multi_s {
    /// The array of ethernet port contexts
    us_rawnet_context_t ethernet_ports[US_RAWNET_MULTI_MAX];
    /// The number of ethernet ports in the list
    int ethernet_port_count;
    /// The routing table
    us_rawnet_multi_mac_item_t routing_table[US_RAWNET_MULTI_MAC_TABLE_SIZE];
    /// The count of the items in the routing_table
    int routing_table_count;
    /// The last time that we did a cleanup
    time_t last_cleanup_time;
} us_rawnet_multi_t;

/// Open all ethernet ports that are link layer capable for the specified
/// Ethertype and up to 2 multicast addresses to listen to
/// Returns the number of usable ethernet ports discovered
int us_rawnet_multi_open(
        us_rawnet_multi_t *self,
        uint16_t ethertype,
        uint8_t const *multicast_address1,
        uint8_t const *multicast_address2);

/// Close all ethernet ports
void us_rawnet_multi_close(us_rawnet_multi_t *self);

/// Join the specified multicast address on all network ports
void us_rawnet_multi_join_multicast(us_rawnet_multi_t *self, uint8_t const mac[6] );

/// Mark an FD_SET with all open socket handles. Returns the largest fd or -1 if none
int us_rawnet_multi_set_fdset( us_rawnet_multi_t *self, fd_set *fdset );

/// Helper funcion for qsort() and bsearch() to compare two entries in the routing table
int us_rawnet_multi_mac_cmp( const void *lhs_, const void *rhs_ );

/// Try to add the tracking for the specified mac address
/// returns 0 on success, -1 if the lookup table is full
int us_rawnet_multi_route_add(
    us_rawnet_multi_t *self,
    uint8_t const mac[6],
    int last_seen_interface,
    time_t last_seen_time );

/// Find the mac address in the routing table and update the stats,
/// If the mac address is not known then add the mac and stats to the routing table
void us_rawnet_multi_route_update_or_add(
    us_rawnet_multi_t *self,
    uint8_t const mac[6],
    int last_seen_interface,
    time_t last_seen_time );

/// Look up the ethernet port to use to send a message to a specified MAC address
/// Returns -1 if if MAC address is multicast or the specified MAC address has not been seen yet.
/// Returning -1 always means that the frame needs to be sent out all etherent ports
int us_rawnet_multi_route_find(
    us_rawnet_multi_t *self,
    uint8_t const mac[6] );

/// Scan through the routing table and remove any stale items. Returns the new count of routing items.
int us_rawnet_multi_route_cleanup(
    us_rawnet_multi_t *self,
    time_t cur_time );

/// Scan through all ethernet ports, try receive an ethernet frame from each.
/// For each ethernet frame received, calls the handler() function
int us_rawnet_multi_receive(
    us_rawnet_multi_t *self,
    time_t cur_time,
    void *context,
    void (*handler)(us_rawnet_multi_t *self, int ethernet_port, void *context, uint8_t *buf, uint16_t len )
    );

/// Assemble a packet from the data,data1,and data2 chunks and send it out all
/// active ethernet ports. Returns the count of packets actually sent.
int us_rawnet_multi_send_all(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     );

/// Assemble a packet from the data,data1,and data2 chunks. Set the DA the ethernet frame to
/// be what the SA of the ethernet frame was, and send it out all active ethernet ports.
/// Returns the count of packets actually sent.
int us_rawnet_multi_send_reply_all(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     );

/// Poll all ethernet ports for incoming packets up to max_poll_count times in a row. Stops
/// polling if no ethernet ports have any frames to receive.
void us_rawnet_multi_rawnet_poll_incoming(
        us_rawnet_multi_t *self,
        time_t cur_time,
        int max_poll_count,
        void *context,
        void (*handler)( us_rawnet_multi_t *self, int ethernet_port, void *context, uint8_t *buf, uint16_t len ) );


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
