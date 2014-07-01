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
#include "us_rawnet_multi.h"

#if defined( US_ENABLE_RAW_ETHERNET )
#if defined( WIN32 )
#include <iphlpapi.h>
#endif

#if defined( __linux__ )
#include <linux/if_link.h>
#endif

#if defined( __APPLE__ ) || defined( __linux__ )
int us_rawnet_multi_open( us_rawnet_multi_t *self,
                          uint16_t ethertype,
                          uint8_t const *multicast_address1,
                          uint8_t const *multicast_address2 )
{
    struct ifaddrs *net_interfaces;
    memset( self, 0, sizeof( *self ) );
    int i;
    self->ethernet_port_count = 0;
    // clear the routing table
    for ( i = 0; i < US_RAWNET_MULTI_MAC_TABLE_SIZE; ++i )
    {
        us_rawnet_multi_mac_item_t *item = &self->routing_table[i];
        item->last_seen_interface = -1;
        memset( item->mac, 0xff, 6 );
        item->last_seen_time = 0;
    }
    self->routing_table_count = 0;
    self->last_cleanup_time = 0;
    // Get a list of all network interfaces
    if ( getifaddrs( &net_interfaces ) == 0 )
    {
        struct ifaddrs *cur = net_interfaces;
        // iterate through them
        while ( cur && self->ethernet_port_count < US_RAWNET_MULTI_MAX )
        {
            // we are only interested in the network interfaces that provide link layer packets
#if defined( __APPLE__ )
            if ( cur->ifa_addr && cur->ifa_addr->sa_family == AF_LINK && cur->ifa_data != 0 && cur->ifa_name[0] == 'e'
                 && cur->ifa_name[1] == 'n' && isdigit( cur->ifa_name[2] ) )
            {
                struct if_data64 *d = (struct if_data64 *)cur->ifa_data;
                if ( d->ifi_type == IFT_ETHER && d->ifi_mtu == 1500 )
                {
#elif defined( __linux__ )
            if ( cur->ifa_addr && cur->ifa_addr->sa_family == AF_PACKET && cur->ifa_name[0] == 'e' && cur->ifa_name[1] == 't'
                 && cur->ifa_name[2] == 'h' )
            {
                {
#endif
                    // us_log_debug("Trying to open ethernet port %s", cur->ifa_name);

                    // found one, try to open a rawnet socket
                    if ( us_rawnet_socket(
                             &self->ethernet_ports[self->ethernet_port_count], ethertype, cur->ifa_name, multicast_address1 )
                         >= 0 )
                    {
                        // success! join the secondary multicast address if necessary
                        if ( multicast_address2 )
                        {
                            us_rawnet_join_multicast( &self->ethernet_ports[self->ethernet_port_count], multicast_address2 );
                        }
                        // count it in our opened port count
                        self->ethernet_port_count++;
                        // us_log_debug("Opened ethernet port %s", cur->ifa_name);
                    }
                }
            }

            // find the next interface in the list
            cur = cur->ifa_next;
        }
        // free the list
        freeifaddrs( net_interfaces );
    }
    else
    {
        us_log_error( "Unable to query interface address list" );
    }
    // return the count of ethernet ports we opened
    return self->ethernet_port_count;
}

void us_rawnet_multi_close( us_rawnet_multi_t * self )
{
    int i;
    for ( i = 0; i < self->ethernet_port_count; ++i )
    {
        us_rawnet_close( &self->ethernet_ports[i] );
    }
}
#elif defined( WIN32 )
#define WORKING_BUFFER_SIZE ( 15000 )
#define MAX_TRIES 3

int us_rawnet_multi_open(
    us_rawnet_multi_t * self, uint16_t ethertype, uint8_t const * multicast_address1, uint8_t const * multicast_address2 )
{

    /* Declare and initialize variables */

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    LONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    family = AF_INET;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do
    {

        pAddresses = (IP_ADAPTER_ADDRESSES *)HeapAlloc( GetProcessHeap(), 0, outBufLen );
        if ( pAddresses == NULL )
        {
            printf( "Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n" );
            return 0;
        }

        dwRetVal = GetAdaptersAddresses( family, flags, NULL, pAddresses, &outBufLen );

        if ( dwRetVal == ERROR_BUFFER_OVERFLOW )
        {
            HeapFree( GetProcessHeap(), 0, pAddresses );
            pAddresses = NULL;
        }
        else
        {
            break;
        }

        Iterations++;

    } while ( ( dwRetVal == ERROR_BUFFER_OVERFLOW ) && ( Iterations < MAX_TRIES ) );

    if ( dwRetVal == NO_ERROR )
    {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while ( pCurrAddresses )
        {

            if ( pCurrAddresses->PhysicalAddressLength == 6 )
            {
                char device_name[4096];
                us_log_debug( "Trying to open ethernet port %s", pCurrAddresses->FriendlyName );
                // found one, try to open a rawnet socket
                sprintf( device_name, "\\Device\\NPF_%s", pCurrAddresses->AdapterName );
                if ( us_rawnet_socket(
                         &self->ethernet_ports[self->ethernet_port_count], ethertype, device_name, multicast_address1 ) >= 0 )
                {
                        // success! join the secondary multicast address if necessary
                        if ( multicast_address2 )
                        {
                            us_rawnet_join_multicast( &self->ethernet_ports[self->ethernet_port_count], multicast_address2 );
                        }
                        // count it in our opened port count
                        self->ethernet_port_count++;
                        us_log_debug( "Opened ethernet port %s", pCurrAddresses->FriendlyName );
                }
            }

            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    else
    {
        us_log_error( "Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal );
        if ( dwRetVal == ERROR_NO_DATA )
        {
            us_log_error( "No addresses were found for the requested parameters\n" );
        }
        else
        {
            if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                dwRetVal,
                                MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                                // Default language
                                ( LPTSTR ) & lpMsgBuf,
                                0,
                                NULL ) )
            {
                us_log_error( "Error: %s", lpMsgBuf );
                LocalFree( lpMsgBuf );
                if ( pAddresses )
                {
                    HeapFree( GetProcessHeap(), 0, pAddresses );
                }
                return 0;
            }
        }
    }

    if ( pAddresses )
    {
        HeapFree( GetProcessHeap(), 0, pAddresses );
    }

    return self->ethernet_port_count;
}


void us_rawnet_multi_close( us_rawnet_multi_t * self )
{
    int i;
    for ( i = 0; i < self->ethernet_port_count; ++i )
    {
        us_rawnet_close( &self->ethernet_ports[i] );
    }
}
#endif

void us_rawnet_multi_join_multicast( us_rawnet_multi_t * self, uint8_t const mac[6] )
{
    int i;
    for ( i = 0; i < self->ethernet_port_count; ++i )
    {
        us_rawnet_join_multicast( &self->ethernet_ports[i], (uint8_t *)mac );
    }
}

int us_rawnet_multi_set_fdset( us_rawnet_multi_t * self, fd_set * fdset )
{
    int largest_fd = -1;
    int i;
    for ( i = 0; i < self->ethernet_port_count; ++i )
    {
        int fd = self->ethernet_ports[i].m_fd;
        if ( fd != -1 )
        {
            FD_SET( fd, fdset );
            if ( fd > largest_fd )
            {
                largest_fd = fd;
            }
        }
    }
    return largest_fd;
}


int us_rawnet_multi_mac_cmp( const void * lhs_, const void * rhs_ )
{
    us_rawnet_multi_mac_item_t const *lhs = (us_rawnet_multi_mac_item_t const *)lhs_;
    us_rawnet_multi_mac_item_t const *rhs = (us_rawnet_multi_mac_item_t const *)rhs_;
    int cmp = 0;

    // when last_seen_interface is set to -1 this means it is not active, push it to the end
    if ( lhs->last_seen_interface == -1 )
    {
        cmp = 1;
    }
    else if ( rhs->last_seen_interface == -1 )
    {
        cmp = -1;
    }
    else
    {
        // both are active, so then we compare mac addresses
        cmp = memcmp( lhs->mac, rhs->mac, 6 );
    }
    return cmp;
}

int us_rawnet_multi_route_add( us_rawnet_multi_t * self, uint8_t const mac[6], int last_seen_interface, time_t last_seen_time )
{
    int r = -1;

    // Do we have space?
    if ( self->routing_table_count < US_RAWNET_MULTI_MAC_TABLE_SIZE )
    {
        // yes, put the new record at the end of the list
        us_rawnet_multi_mac_item_t *item = &self->routing_table[self->routing_table_count++];
        memcpy( item->mac, mac, 6 );
        item->last_seen_interface = last_seen_interface;
        item->last_seen_time = last_seen_time;
        item->last_changed_time = last_seen_time;
        // Then, sort the list so we can do a binary search on it later
        qsort( self->routing_table, self->routing_table_count, sizeof( self->routing_table[0] ), us_rawnet_multi_mac_cmp );
        r = 0;
    }
    return r;
}

void us_rawnet_multi_route_update_or_add(
    us_rawnet_multi_t * self, uint8_t const mac[6], int last_seen_interface, time_t last_seen_time )
{
    void *p;
    // create a routing table item to compare with
    us_rawnet_multi_mac_item_t lhs;
    lhs.last_seen_interface = 0;
    lhs.last_seen_time = 0;
    lhs.last_changed_time = 0;
    memcpy( lhs.mac, mac, 6 );
    // try find it
    p = bsearch(
        &lhs, self->routing_table, self->routing_table_count, sizeof( self->routing_table[0] ), us_rawnet_multi_mac_cmp );
    if ( p )
    {
        // found it, so just update the iterface index and last_seen time
        // but only if the last change time was not too recent
        us_rawnet_multi_mac_item_t *item = (us_rawnet_multi_mac_item_t *)p;
        if ( item->last_seen_interface != last_seen_interface && ( last_seen_time - item->last_changed_time )
                                                                 > US_RAWNET_MULTI_ROUTE_FLAP_RATE_IN_SECONDS )
        {
            us_log_debug( "updating routing for MAC: %02x:%02x:%02x:%02x:%02x:%02x old_if=%2d new_if=%2d time=%u",
                          mac[0],
                          mac[1],
                          mac[2],
                          mac[3],
                          mac[4],
                          mac[5],
                          item->last_seen_interface,
                          last_seen_interface,
                          last_seen_time );
            item->last_seen_interface = last_seen_interface;
            item->last_changed_time = last_seen_time;
        }
        item->last_seen_time = last_seen_time;
    }
    else
    {
        // we do not have a record for this mac address, so add it
        us_log_debug( "adding routing for MAC: %02x:%02x:%02x:%02x:%02x:%02x if=%2d time=%u",
                      mac[0],
                      mac[1],
                      mac[2],
                      mac[3],
                      mac[4],
                      mac[5],
                      last_seen_interface,
                      last_seen_time );
        us_rawnet_multi_route_add( self, mac, last_seen_interface, last_seen_time );
    }
}


int us_rawnet_multi_route_find( us_rawnet_multi_t * self, uint8_t const mac[6] )
{
    ptrdiff_t port = -1;

    // is the mac address unicast? multicast always goes out all ports.
    if ( ( mac[0] & 1 ) == 0 )
    {
        void *p;
        // yes, it is unicast. We will look it up.
        // create a routing table item to compare with
        us_rawnet_multi_mac_item_t lhs;
        lhs.last_seen_interface = 0;
        lhs.last_seen_time = 0;
        memcpy( lhs.mac, mac, 6 );
        // try find it
        p = bsearch(
            &lhs, self->routing_table, self->routing_table_count, sizeof( self->routing_table[0] ), us_rawnet_multi_mac_cmp );
        if ( p )
        {
            us_rawnet_multi_mac_item_t *item = (us_rawnet_multi_mac_item_t *)p;
            port = item - self->routing_table;
        }
    }
    return (int)port;
}

int us_rawnet_multi_route_cleanup( us_rawnet_multi_t * self, time_t cur_time )
{
    int expired_items = 0;

    // Is it time to try to cleanup?
    if ( cur_time - self->last_cleanup_time > US_RAWNET_MULTI_ROUTE_CLEANUP_RATE_IN_SECONDS )
    {
        // yes!
        int i;

        // mark this cleanup time
        self->last_cleanup_time = cur_time;

        // go through all items,
        for ( i = 0; i < self->routing_table_count; ++i )
        {
            us_rawnet_multi_mac_item_t *item = &self->routing_table[i];
            // is the item inactive or did the timeout expire?
            if ( item->last_seen_interface == -1 || ( cur_time - item->last_seen_time )
                                                    > US_RAWNET_MULTI_MAC_ROUTE_EXPIRY_TIME_IN_SECONDS )
            {
                us_log_debug( "expiring routing for MAC: %02x:%02x:%02x:%02x:%02x:%02x if=%2d time=%u",
                              item->mac[0],
                              item->mac[1],
                              item->mac[2],
                              item->mac[3],
                              item->mac[4],
                              item->mac[5],
                              item->last_seen_interface,
                              item->last_seen_time );
                // yes, mark it inactive
                item->last_seen_interface = -1;
                // count how many we marked inactive
                expired_items++;
            }
        }

        // Did we mark any inactive?
        if ( expired_items > 0 )
        {
            // yes, so we need to sort them to the bottom of the list
            qsort( self->routing_table, self->routing_table_count, sizeof( self->routing_table[0] ), us_rawnet_multi_mac_cmp );
            // and reduce our count by that number
            self->routing_table_count -= expired_items;
        }
    }
    // return the current count of active routing table items
    return self->routing_table_count;
}


int us_rawnet_multi_receive(
    us_rawnet_multi_t * self,
    time_t cur_time,
    void * context,
    void ( *handler )( us_rawnet_multi_t * self, int ethernet_port, void * context, uint8_t * buf, uint16_t len ) )
{
    int receive_count = 0;
    int i;
    for ( i = 0; i < self->ethernet_port_count; ++i )
    {
        uint8_t data[1514];
        ssize_t r = us_rawnet_recv( &self->ethernet_ports[i], &data[6], &data[0], &data[14], sizeof( data ) - 14 );

        if ( r > 1 )
        {
            // returned length includes frame header
            r += 14;
            // manually stick ethertype in there as well
            data[12 + 0] = ( self->ethernet_ports[i].m_ethertype >> 8 ) & 0xff;
            data[12 + 1] = ( self->ethernet_ports[i].m_ethertype >> 0 ) & 0xff;
            // update the routing table based on this source mac address being seen on this interface
            us_rawnet_multi_route_update_or_add( self, &data[6], i, cur_time );
            // call the handler
            handler( self, i, context, data, r );
            // increase the receive count
            receive_count++;
        }
    }
    // clean up any stale routes
    us_rawnet_multi_route_cleanup( self, cur_time );
    return receive_count;
}

int us_rawnet_multi_send_all( us_rawnet_multi_t * self,
                              uint8_t * data,
                              uint16_t len,
                              uint8_t const * data1,
                              uint16_t len1,
                              uint8_t const * data2,
                              uint16_t len2 )
{
    int frames_sent = 0;
    int i;
    uint8_t buf[1514];
    int interface_to_send_to = -1;
    uint16_t total_len = len + len1 + len2;

    // Put pieces together into one buffer, with frame header
    memcpy( buf, data, len );
    if ( data1 && len1 )
    {
        memcpy( buf + len, data1, len1 );
    }
    if ( data2 && len2 )
    {
        memcpy( buf + len + len1, data2, len2 );
    }

    // try find out which interface we need to send to, or all, based on the routing table
    interface_to_send_to = us_rawnet_multi_route_find( self, &buf[0] );
    if ( interface_to_send_to == -1 )
    {
        // send to all
        for ( i = 0; i < self->ethernet_port_count; ++i )
        {
            ssize_t sent = us_rawnet_send( &self->ethernet_ports[i], &buf[0], &buf[14], total_len - 14 );
            if ( sent > 0 )
            {
                frames_sent++;
            }
        }
    }
    else
    {
        // send to one
        ssize_t sent = us_rawnet_send( &self->ethernet_ports[interface_to_send_to], &buf[0], &buf[14], total_len - 14 );
        if ( sent > 0 )
        {
            frames_sent++;
        }
    }

    return frames_sent;
}

int us_rawnet_multi_send_reply_all( us_rawnet_multi_t * self,
                                    uint8_t * data,
                                    uint16_t len,
                                    uint8_t const * data1,
                                    uint16_t len1,
                                    uint8_t const * data2,
                                    uint16_t len2 )
{
    // Send reply to whoever sent me this frame
    int frames_sent = 0;
    int i;
    uint8_t buf[1514];
    int interface_to_send_to = -1;
    uint16_t total_len = len + len1 + len2;

    // Put pieces together into one buffer, with frame header
    memcpy( buf, data, len );
    if ( data1 && len1 )
    {
        memcpy( buf + len, data1, len1 );
    }
    if ( data2 && len2 )
    {
        memcpy( buf + len + len1, data2, len2 );
    }

    // try find out which interface we need to send to, or all, based on the routing table
    interface_to_send_to = us_rawnet_multi_route_find( self, &buf[0] );
    if ( interface_to_send_to == -1 )
    {
        // send to all
        for ( i = 0; i < self->ethernet_port_count; ++i )
        {
            ssize_t sent = us_rawnet_send( &self->ethernet_ports[i], &buf[6], &buf[14], total_len - 14 );
            if ( sent > 0 )
            {
                frames_sent++;
            }
        }
    }
    else
    {
        // send to one
        ssize_t sent = us_rawnet_send( &self->ethernet_ports[interface_to_send_to], &buf[6], &buf[14], total_len - 14 );
        if ( sent > 0 )
        {
            frames_sent++;
        }
    }

    return frames_sent;
}


void us_rawnet_multi_rawnet_poll_incoming(
    us_rawnet_multi_t * self,
    time_t cur_time,
    int max_poll_count,
    void * context,
    void ( *handler )( us_rawnet_multi_t * self, int ethernet_port, void * context, uint8_t * buf, uint16_t len ) )
{
    int poll_count = 0;
    int zero_count = 0;
    for ( poll_count = 0; poll_count < max_poll_count; ++poll_count )
    {
        if ( us_rawnet_multi_receive( self, cur_time, context, handler ) == 0 )
        {
            if ( zero_count++ > 2 )
            {
                break;
            }
        }
    }
}

#endif
