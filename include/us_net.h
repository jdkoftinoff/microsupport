#ifndef US_NET_H
#define US_NET_H

/*
 Copyright (c) 2013, J.D. Koftinoff Software, Ltd.
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

#ifndef US_WORLD_H
#include "us_world.h"
#endif

#include "us_allocator.h"
#include "us_buffer.h"
#include "us_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** \addtogroup us_net
     */
    /*@{*/
#if defined(US_CONFIG_POSIX) || defined(US_CONFIG_WIN32)
    struct addrinfo *
    us_net_get_addrinfo (
        const char *ip_addr,
        const char *ip_port,
        int type,
        bool for_server
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
        const struct addrinfo *ai,
        bool do_bind
    );

    int
    us_net_create_udp_socket_host(
            const char *localaddr_host,
            const char *localaddr_port,
            bool do_bind
      );

    int
    us_net_create_tcp_socket (
        const struct addrinfo *ai,
        bool do_bind
    );

    int
    us_net_create_tcp_socket_host(
            const char *localaddr_host,
            const char *localaddr_port,
            bool do_bind
      );

    int us_net_create_multicast_rx_udp_socket (
        struct addrinfo *listenaddr,
        const struct addrinfo *multicastgroup,
        const char *interface_name
    );

    int us_net_create_multicast_tx_udp_socket (
        struct addrinfo *localaddr,
        const struct addrinfo *multicastgroup,
        const char *interface_name
    );

    int us_net_create_multicast_udp_socket(
            struct addrinfo *localaddr,
            const struct addrinfo *multicastgroup,
            const char *interface_name,
            bool tx
            );

    int us_net_create_multicast_udp_socket_host(
            const char *localaddr_host,
            const char *localaddr_port,
            const char *multicast_host,
            const char *multicast_port,
            const char *interface_name,
            bool tx
            );

    void
    us_net_timeout_add (
        struct timeval *result,
        const struct timeval *cur_time,
        uint32_t microseconds_to_add
    );

    bool
    us_net_timeout_calc (
        struct timeval *result,
        const struct timeval *cur_time,
        const struct timeval *next_time
    );

    bool
    us_net_timeout_hit (
        const struct timeval *cur_time,
        const struct timeval *next_time
    );

    bool
    us_net_blocking_send(
        int sock,
        const void *data,
        int32_t len
    );

    /** us_net_wait_readable
     *
     *  Utility function for simple (and inefficent) select() calls.
     *  Given list of file handles it waits for up to timeout_ms time in milleconds
     *  for one of the file handles to become readable.
     *  timeout_ms may be -1 for infinite timeout.
     *  return value is a single file handle that is readable.
     *  return value is -1 if timeout is hit.
     *  return value is -2 if error occurred
     */
    int us_net_wait_readable(
        int timeout_ms,
        int fd_count,
        ...
    );

    int us_net_wait_readable_list(
        struct timeval *cur_time,
        struct timeval *wake_up_time,
        uint32_t next_wake_up_delta_time_milliseconds,
        int fd_count,
        const int *fds
        );

#endif
#ifdef __cplusplus
}
#endif


#endif
