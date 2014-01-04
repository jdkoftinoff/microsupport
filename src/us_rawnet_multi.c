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
#include "us_rawnet_multi.h"

#if defined(__APPLE__)
bool us_rawnet_multi_open(
        us_rawnet_multi_t *self,
        uint16_t ethertype,
        uint8_t const *multicast_address1,
        uint8_t const *multicast_address2) {
    struct ifaddrs *net_interfaces;
    self->ethernet_port_count = 0;
    // Get a list of all network interfaces
    if( getifaddrs(&net_interfaces)==0 ) {
        struct ifaddrs *cur = net_interfaces;
        // iterate through them
        while ( cur && self->ethernet_port_count<US_RAWNET_MULTI_MAX ) {
            // we are only interested in the network interfaces that provide link layer packets
            if( cur->ifa_addr
                && cur->ifa_addr->sa_family == AF_LINK
                && cur->ifa_data!=0
                ) {
                struct if_data64 *d = (struct if_data64 *)cur->ifa_data;
                if( d->ifi_type == IFT_ETHER && d->ifi_mtu==1500) {
                    us_log_debug("Trying to open ethernet port %s", cur->ifa_name);

                    // found one, try to open a rawnet socket
                    if( us_rawnet_socket(
                            &self->ethernet_ports[ self->ethernet_port_count ],
                            ethertype,
                            cur->ifa_name,
                            multicast_address1 )>=0 ) {
                        // success! join the secondary multicast address if necessary
                        if( multicast_address2 ) {
                            us_rawnet_join_multicast(&self->ethernet_ports[self->ethernet_port_count], multicast_address2 );
                        }
                        // count it in our opened port count
                        self->ethernet_port_count++;
                        us_log_debug("Opened ethernet port %s", cur->ifa_name);
                    }
                }
            }
            // find the next interface in the list
            cur=cur->ifa_next;
        }
        // free the list
        freeifaddrs(net_interfaces);
    } else {
//        fprintf(stderr,"Unable to query interface address list");
    }
    // return true if we we able to open at least one network interface
    return self->ethernet_port_count>0;
}

void us_rawnet_multi_close(us_rawnet_multi_t *self) {
    int i;
    for( i=0; i<self->ethernet_port_count; ++i ) {
        us_rawnet_close(&self->ethernet_ports[i]);
    }
}
#elif defined(__linux__)
// TODO: linux version
#elif defined(WIN32)
// TODO: windows version
#endif

int us_rawnet_multi_receive(
    us_rawnet_multi_t *self,
    void *context,
    bool (*handler)(us_rawnet_multi_t *self, void *context, uint8_t *buf, uint16_t len )
    ) {
    int receive_count=0;
    int i;
    for( i=0; i<self->ethernet_port_count; ++i ) {
        uint8_t data[1514];
        int r = us_rawnet_recv(
                &self->ethernet_ports[i],
                &data[6],
                &data[0],
                &data[14],
                sizeof(data) - 14);

        if (r > 1) {
            // returned length includes frame header
            r += 14;
            // manually stick ethertype in there as well
            data[12 + 0] = (self->ethernet_ports[i].m_ethertype >> 8) & 0xff;
            data[12 + 1] = (self->ethernet_ports[i].m_ethertype >> 0) & 0xff;
            handler(self,context,data,r);
            receive_count++;
        }
    }
    return receive_count;
}

int us_rawnet_multi_send(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     ) {
    int frames_sent=0;
    int i;
    uint8_t buf[1514];

    // Put pieces together into one buffer, with frame header
    memcpy(buf, data, len);
    if (data1 && len1) {
        memcpy(buf + len, data1, len1);
    }
    if (data2 && len2) {
        memcpy(buf + len + len1, data2, len2);
    }
    uint16_t total_len = len + len1 + len2;
    for( i=0; i<self->ethernet_port_count; ++i ) {
        ssize_t sent = us_rawnet_send(
                            &self->ethernet_ports[i],
                            &buf[0],
                            &buf[14],
                            total_len - 14);
        if( sent>0 ) {
            frames_sent++;
        }
    }
    return frames_sent;
}

int us_rawnet_multi_send_reply(
    us_rawnet_multi_t *self,
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     ) {
    // Send reply to whoever sent me this frame
    // Put pieces together into one buffer, with frame header
    int frames_sent=0;
    int i;
    uint8_t buf[1514];

    // Put pieces together into one buffer, with frame header
    memcpy(buf, data, len);
    if (data1 && len1) {
        memcpy(buf + len, data1, len1);
    }
    if (data2 && len2) {
        memcpy(buf + len + len1, data2, len2);
    }
    uint16_t total_len = len + len1 + len2;
    for( i=0; i<self->ethernet_port_count; ++i ) {
        ssize_t sent = us_rawnet_send(
                            &self->ethernet_ports[i],
                            &buf[6],
                            &buf[14],
                            total_len - 14);
        if( sent>0 ) {
            frames_sent++;
        }
    }
    return frames_sent;
}


void us_rawnet_multi_rawnet_poll_incoming(
        us_rawnet_multi_t *self,
        int max_poll_count,
        void *context,
        bool (*handler)( us_rawnet_multi_t *self, void *context, uint8_t *buf, uint16_t len ) ) {
    int poll_count=0;
    for( poll_count=0; poll_count<max_poll_count; ++poll_count ) {
        if( us_rawnet_multi_receive(self,context,handler)==0 ) {
            break;
        }
    }
}
