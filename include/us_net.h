#ifndef US_NET_H
#define US_NET_H

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

#include "us_allocator.h"
#include "us_buffer.h"
#include "us_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

    /** \addtogroup us_net
     */
    /*@{*/
#if defined(US_CONFIG_POSIX)
    struct addrinfo *
    us_net_get_addrinfo (
        const char *ip_addr,
        const char *ip_port,
        int type
    );
    
    bool us_net_get_nameinfo (
        struct addrinfo *ai,
        char *hostname_buf,
        int hostname_buf_len,
        char *serv_buf,
        int serv_buf_len
    );
    
    int
    us_net_create_udp_socket (
        struct addrinfo *ai,
        bool do_bind
    );
    
    int
    us_net_create_tcp_socket (
        struct addrinfo *ai,
        bool do_bind
    );
    
    int us_net_create_multicast_rx_udp_socket (
        struct addrinfo *listenaddr,
        struct addrinfo *multicastgroup,
        const char *interface_name
    );
    
    int us_net_create_multicast_tx_udp_socket (
        struct addrinfo *localaddr,
        struct addrinfo *multicastgroup,
        const char *interface_name
    );
    
    
    void
    us_net_timeout_add (
        struct timeval *result,
        struct timeval *cur_time,
        uint32_t microseconds_to_add
    );
    
    bool
    us_net_timeout_calc (
        struct timeval *result,
        struct timeval *cur_time,
        struct timeval *next_time
    );
    
    bool
    us_net_timeout_hit (
        struct timeval *cur_time,
        struct timeval *next_time
    );
    
#endif
    
    typedef struct us_net_handler_s
    {
        us_allocator_t *m_allocator;
        int m_socket;
        us_queue_t *m_incoming_queue;
        us_queue_t *m_outgoing_queue;
        
        void ( *destroy ) ( struct us_net_handler_s * );
        bool ( *data_was_read ) ( struct us_net_handler_s * );
        bool ( *data_was_written ) ( struct us_net_handler_s * );
        bool ( *timeout_hit ) ( struct us_net_handler_s * );
        
    } us_net_handler_t;
    
    us_net_handler_t *
    us_net_handler_create (
        us_allocator_t *allocator,
        int32_t incoming_queue_size,
        int32_t outgoing_queue_size
    );
    
    void
    us_net_handler_destroy (
        us_net_handler_t *self
    );
    
    
    typedef struct us_net_reactor_s
    {
        us_allocator_t *m_allocator;
        
        void ( *destroy ) ( struct us_net_reactor_s * );
#if US_ENABLE_BSD_SOCKETS
        fd_set m_readable;
        fd_set m_writable;
        struct timeval m_timeout;
#elif US_ENABLE_UIPV4_STACK
        /* TODO: */
#endif
        int32_t m_max_handlers;
        us_net_handler_t **m_handlers;
    } us_net_reactor_t;
    
    us_net_reactor_t *
    us_net_reactor_create (
        us_allocator_t *allocator,
        int32_t max_handlers
    );
    
    bool
    us_net_reactor_add_handler (
        us_net_reactor_t *self,
        us_net_handler_t *handler
    );
    
    bool
    us_net_reactor_remove_handler (
        us_net_reactor_t *self,
        us_net_handler_t *handler
    );
    
    bool
    us_net_reactor_find_handler (
        us_net_reactor_t *self,
        int sock
    );
    
    
    /*@}*/
    
#ifdef __cplusplus
}
#endif


#endif
