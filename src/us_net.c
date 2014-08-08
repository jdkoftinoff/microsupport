#include "us_world.h"
#include "us_net.h"
#include "us_logger.h"

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

#if US_ENABLE_RAW_ETHERNET
#include "us_rawnet.h"
#endif

#if US_ENABLE_BSD_SOCKETS==1 || defined(US_CONFIG_WIN32)

struct addrinfo *us_net_get_addrinfo( const char *ip_addr, const char *ip_port, int type, bool for_server )
{
    int e;
    struct addrinfo *ai;
    struct addrinfo hints;
    memset( &hints, '\0', sizeof( hints ) );

#if defined( AI_NUMERICHOST )
    hints.ai_flags |= AI_NUMERICHOST;
#endif

#if defined( AI_NUMERICSERV )
    hints.ai_flags |= AI_NUMERICSERV;
#endif

    if ( for_server )
        hints.ai_flags |= AI_PASSIVE;

    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = 0;

    if ( ip_addr != 0 && *ip_addr == '\0' )
        ip_addr = 0;
    if ( ip_port != 0 && *ip_port == '\0' )
        ip_port = 0;

    do
    {
        e = getaddrinfo( ip_addr, ip_port, &hints, &ai );
    } while ( e == EAI_AGAIN );

    if ( e != 0 )
    {
        us_log_error( "getaddrinfo: %s", gai_strerror( e ) );
        ai = 0;
    }

    return ai;
}

bool us_net_get_nameinfo( struct addrinfo *ai, char *hostname_buf, int hostname_buf_len, char *serv_buf, int serv_buf_len )
{
    bool r = false;
    if ( getnameinfo( ai->ai_addr,
                      ai->ai_addrlen,
                      hostname_buf,
                      hostname_buf_len,
                      serv_buf,
                      serv_buf_len,
                      NI_NUMERICHOST | NI_NUMERICSERV ) == 0 )
    {
        r = true;
    }
    return r;
}

bool us_net_convert_sockaddr_to_string( struct sockaddr const *addr, socklen_t addrlen, char *buf, size_t buflen )
{

    bool r = false;
    char hostbuf[512];
    char servbuf[512];

    *buf = 0;
    if ( addr && addrlen > 6 )
    {
#if US_ENABLE_RAW_ETHERNET
        if ( addr->sa_family == US_AF_LINK )
        {
            if ( addr->sa_family == US_AF_LINK )
            {
                uint8_t mac[6];
                memcpy( mac, us_sockaddr_dl_get_mac( addr ), 6 );
                us_snprintf( buf, buflen - 1, "[%02x:%02x:%02x:%02x:%02x:%02x]", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
                r = true;
            }
        }
        else
#endif
        {
            int e = getnameinfo( addr,
                                 addrlen,
                                 hostbuf,
                                 sizeof( hostbuf ),
                                 servbuf,
                                 sizeof( servbuf ),
                                 NI_NUMERICHOST | NI_NUMERICSERV | NI_DGRAM );

            if ( e == 0 )
            {
                us_snprintf( buf, buflen - 1, "[%s]:%s", hostbuf, servbuf );
                r = true;
            }
            else
            {
                us_log_error( "getnameinfo sa_family %d error: %s", addr->sa_family, gai_strerror( e ) );
            }
        }
    }
    return r;
}

int us_net_create_udp_socket( const struct addrinfo *ai, bool do_bind )
{
    int r = -1;
    int s = -1;
    bool broadcast = false;
    if ( ai )
    {
        s = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
        if ( s >= 0 )
        {
            int on = 1;
            r = s;
            if ( broadcast )
            {
                if ( setsockopt( s, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof( on ) ) == -1 )
                {
                    us_log_error( "setsockopt SO_BROADCAST: %s", strerror( errno ) );
                    closesocket( s );
                    return -1;
                }
            }
            if ( do_bind )
            {
                if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof( on ) ) == -1 )
                {
                    us_log_error( "setsockopt SO_REUSEADDR: %s", strerror( errno ) );
                    closesocket( s );
                    return -1;
                }
                if ( bind( s, ai->ai_addr, ai->ai_addrlen ) < 0 )
                {
                    us_log_error( "bind: %s", strerror( errno ) );
                    closesocket( s );
                    return -1;
                }
            }
        }
        else
        {
            us_log_error( "socket: %s", strerror( errno ) );
        }
        return r;
    }
    if ( r == -1 && s != -1 )
    {
        closesocket( s );
    }
    return s;
}

int us_net_create_udp_socket_host( const char *localaddr_host, const char *localaddr_port, bool do_bind )
{
    int fd = 0;
    struct addrinfo *localaddr = 0;

    localaddr = us_net_get_addrinfo( localaddr_host, localaddr_port, SOCK_DGRAM, true );

    fd = us_net_create_udp_socket( localaddr, do_bind );
    freeaddrinfo( localaddr );
    return fd;
}

int us_net_create_tcp_socket_host( const char *localaddr_host, const char *localaddr_port, bool do_bind )
{
    int fd = 0;
    struct addrinfo *localaddr = 0;

    localaddr = us_net_get_addrinfo( localaddr_host, localaddr_port, SOCK_STREAM, true );

    fd = us_net_create_tcp_socket( localaddr, do_bind );
    freeaddrinfo( localaddr );
    return fd;
}

int us_net_create_multicast_rx_udp_socket( struct addrinfo *listenaddr,
                                           const struct addrinfo *multicastgroup,
                                           const char *interface_name )
{
    int s = -1;
    int if_index = 0;
    if ( interface_name && *interface_name != '\0' )
        if_index = if_nametoindex( interface_name );
    if ( listenaddr == 0 )
    {
        if ( multicastgroup->ai_family == PF_INET6 )
        {
            listenaddr = us_net_get_addrinfo( "0::0", 0, SOCK_DGRAM, true );
            if ( listenaddr )
            {
                ( (struct sockaddr_in6 *)listenaddr->ai_addr )->sin6_port
                    = ( (struct sockaddr_in6 *)multicastgroup->ai_addr )->sin6_port;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            listenaddr = us_net_get_addrinfo( "0.0.0.0", 0, SOCK_DGRAM, true );
            if ( listenaddr )
            {
                ( (struct sockaddr_in *)listenaddr->ai_addr )->sin_port
                    = ( (struct sockaddr_in *)multicastgroup->ai_addr )->sin_port;
            }
            else
            {
                return -1;
            }
        }
    }
    s = socket( listenaddr->ai_family, listenaddr->ai_socktype, listenaddr->ai_protocol );
    if ( s < 0 )
    {
        us_log_error( "socket: %s", strerror( errno ) );
        closesocket( s );
        return -1;
    }
    {
        int on = 1;
        if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof( on ) ) == -1 )
        {
            us_log_error( "setsockopt SO_REUSEADDR: %s", strerror( errno ) );
            closesocket( s );
            return -1;
        }
#if defined( SO_REUSEPORT )
        if ( setsockopt( s, SOL_SOCKET, SO_REUSEPORT, (const char *)&on, sizeof( on ) ) == -1 )
        {
            us_log_error( "setsockopt SO_REUSEPORT: %s", strerror( errno ) );
            closesocket( s );
            return -1;
        }
#endif
    }
    if ( bind( s, listenaddr->ai_addr, listenaddr->ai_addrlen ) != 0 )
    {
        closesocket( s );
        us_log_error( "bind: %s", strerror( errno ) );
        return -1;
    }
    if ( multicastgroup->ai_family == PF_INET6 )
    {
        struct ipv6_mreq multicast_request;
        multicast_request.ipv6mr_multiaddr = ( (struct sockaddr_in6 *)multicastgroup->ai_addr )->sin6_addr;
        multicast_request.ipv6mr_interface = if_index;
        if ( setsockopt( s, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *)&multicast_request, sizeof( multicast_request ) ) != 0 )
        {
            us_log_error( "setsockopt IPV6_JOIN_GROUP: %s", strerror( errno ) );
            closesocket( s );
            return -1;
        }
        if ( setsockopt( s, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&if_index, sizeof( if_index ) ) < 0 )
        {
            us_log_error(
                "socket: %d unable to IPV6_MULTICAST_IF for multicast via interface %s (%d)", s, interface_name, if_index );
            closesocket( s );
            return -1;
        }
    }
    else
    {
        if ( multicastgroup->ai_family == PF_INET )
        {
            struct in_addr in_local;
            in_local.s_addr = ( (struct sockaddr_in *)listenaddr->ai_addr )->sin_addr.s_addr;
            struct ip_mreq multicast_request;
            memcpy( &multicast_request.imr_multiaddr,
                    &( (struct sockaddr_in *)( multicastgroup->ai_addr ) )->sin_addr,
                    sizeof( multicast_request.imr_multiaddr ) );
            multicast_request.imr_interface.s_addr = in_local.s_addr;
            if ( setsockopt( s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multicast_request, sizeof( multicast_request ) ) != 0 )
            {
                us_log_error( "setsockopt IP_ADD_MEMBERSHIP: %s", strerror( errno ) );
                closesocket( s );
                return -1;
            }
            if ( setsockopt( s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&in_local, sizeof( in_local ) ) != 0 )
            {
                us_log_error( "setsockopt IP_MULTICAST_IF %s", strerror( errno ) );
                closesocket( s );
                return -1;
            }
        }
    }
    return s;
}

int us_net_create_multicast_tx_udp_socket( struct addrinfo *localaddr,
                                           const struct addrinfo *multicastgroup,
                                           const char *interface_name )
{
    int s = -1;
    int if_index = 0;
    if ( !localaddr )
    {
        if ( multicastgroup && multicastgroup->ai_family == PF_INET6 )
        {
            localaddr = us_net_get_addrinfo( "0::0", 0, SOCK_DGRAM, false );
        }
        else
        {
            localaddr = us_net_get_addrinfo( "0.0.0.0", 0, SOCK_DGRAM, false );
        }
    }
    s = socket( localaddr->ai_family, localaddr->ai_socktype, localaddr->ai_protocol );
    if ( s < 0 )
    {
        us_log_error( "socket: %s", strerror( errno ) );
        return -1;
    }
    if ( multicastgroup && multicastgroup->ai_family == PF_INET6 )
    {
        if ( interface_name && *interface_name != '\0' )
        {
            if_index = if_nametoindex( interface_name );
            if ( if_index == 0 )
            {
                us_log_error( "socket: %d interface_name %s unknown", s, interface_name );
                closesocket( s );
                return -1;
            }
            if ( setsockopt( s, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&if_index, sizeof( if_index ) ) < 0 )
            {
                us_log_error(
                    "socket: %d unable to IPV6_MULTICAST_IF for multicast via interface %s (%d)", s, interface_name, if_index );
                closesocket( s );
                return -1;
            }
        }
    }
    if ( localaddr && localaddr->ai_family == PF_INET )
    {
        struct in_addr in_local;
        in_local.s_addr = ( (struct sockaddr_in *)localaddr->ai_addr )->sin_addr.s_addr;
        if ( setsockopt( s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&in_local, sizeof( in_local ) ) != 0 )
        {
            us_log_error( "setsockopt IP_MULTICAST_IF %s", strerror( errno ) );
            closesocket( s );
            return -1;
        }
    }
    return s;
}

int us_net_create_multicast_udp_socket( struct addrinfo *localaddr,
                                        const struct addrinfo *multicastgroup,
                                        const char *interface_name,
                                        bool tx )
{
    int fd;
    if ( tx )
    {
        fd = us_net_create_multicast_tx_udp_socket( localaddr, multicastgroup, interface_name );
    }
    else
    {
        fd = us_net_create_multicast_rx_udp_socket( localaddr, multicastgroup, interface_name );
    }

    return fd;
}

int us_net_create_multicast_udp_socket_host( const char *localaddr_host,
                                             const char *localaddr_port,
                                             const char *multicast_host,
                                             const char *multicast_port,
                                             const char *interface_name,
                                             bool tx )
{
    int fd = 0;
    struct addrinfo *localaddr = 0;
    struct addrinfo *multicastaddr = 0;

    localaddr = us_net_get_addrinfo( localaddr_host, localaddr_port, SOCK_DGRAM, true );

    multicastaddr = us_net_get_addrinfo( multicast_host, multicast_port, SOCK_DGRAM, false );

    fd = us_net_create_multicast_udp_socket( localaddr, multicastaddr, interface_name, tx );

    freeaddrinfo( localaddr );
    freeaddrinfo( multicastaddr );

    return fd;
}

int us_net_create_tcp_socket( const struct addrinfo *ai, bool do_bind )
{
    int r = -1;
    int s = -1;
    if ( ai )
    {
        s = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
        if ( s >= 0 )
        {
            r = s;
            if ( do_bind )
            {
                if ( bind( s, ai->ai_addr, ai->ai_addrlen ) < 0 )
                {
                    us_log_error( "bind: %s", strerror( errno ) );
                    r = -1;
                }
            }
        }
        else
        {
            us_log_error( "socket: %s", strerror( errno ) );
        }
        return r;
    }
    if ( r == -1 && s != -1 )
    {
        closesocket( s );
    }
    return s;
}

void us_net_timeout_add( struct timeval *result, const struct timeval *cur_time, uint32_t microseconds_to_add )
{
    int32_t secs = microseconds_to_add / 1000000;
    int32_t micros = microseconds_to_add % 1000000;
    result->tv_sec = cur_time->tv_sec + secs;
    result->tv_usec = cur_time->tv_usec + micros;
    if ( result->tv_usec >= 1000000 )
    {
        result->tv_usec -= 1000000;
        result->tv_sec++;
    }
}

bool us_net_timeout_calc( struct timeval *result, const struct timeval *cur_time, const struct timeval *next_time )
{
    bool r = false;
    if ( us_net_timeout_hit( cur_time, next_time ) )
    {
        /* timeout was hit already, so hack it to 1 sec */
        result->tv_sec = 1;
        result->tv_usec = 0;
    }
    else
    {
        result->tv_sec = next_time->tv_sec - cur_time->tv_sec;
        result->tv_usec = next_time->tv_usec - cur_time->tv_usec;
        if ( result->tv_usec < 0 )
        {
            result->tv_usec += 1000000;
            result->tv_sec--;
        }
        r = true;
    }
    return r;
}

bool us_net_timeout_hit( const struct timeval *cur_time, const struct timeval *next_time )
{
    bool r = false;
    if ( cur_time->tv_sec > next_time->tv_sec
         || ( cur_time->tv_sec == next_time->tv_sec && cur_time->tv_usec >= next_time->tv_usec ) )
    {
        r = true;
    }
    return r;
}

bool us_net_blocking_send( int sock, const void *data_, ssize_t len )
{
    ssize_t todo = len;
    const uint8_t *data = (const uint8_t *)data_;
    ssize_t cnt;
    while ( todo > 0 )
    {
        do
        {
            cnt = send( sock, data, todo, 0 );
        } while ( cnt < 0 && errno == EINTR );
        if ( cnt <= 0 )
            break;
        todo -= cnt;
        data += cnt;
    }
    return ( todo == 0 );
}

int us_net_wait_readable( int timeout_ms, int fd_count, ... )
{
    int r = -1;
    int n = 0;
    fd_set readable_set;
    struct timeval tv_timeout;
    int max_fd = -1;
    int i;
    va_list ap;
    va_start( ap, fd_count );
    FD_ZERO( &readable_set );
    tv_timeout.tv_usec = ( timeout_ms * 1000 ) % 1000;
    tv_timeout.tv_sec = ( timeout_ms / 1000 );
    for ( i = 0; i < fd_count; ++i )
    {
        int fd = va_arg( ap, int );
        if ( fd != -1 )
        {
            FD_SET( fd, &readable_set );
            if ( fd > max_fd )
            {
                max_fd = fd;
            }
        }
    }
    do
    {
        n = select( max_fd + 1, &readable_set, 0, 0, timeout_ms < 0 ? 0 : &tv_timeout );
    } while ( n < 0 && ( errno == EINTR ) );

    va_end( ap );

    if ( n < 0 )
    {
        r = -2; /* Error */
    }
    else if ( n == 0 )
    {
        r = -1; /* Timeout */
    }
    else if ( n > 0 )
    {
        r = -1;
        for ( i = 0; i < max_fd + 1; i++ )
        {
            if ( FD_ISSET( i, &readable_set ) )
            {
                r = i;
                break;
            }
        }
    }
    return r;
}

int us_net_wait_readable_list( struct timeval *cur_time,
                               struct timeval *wake_up_time,
                               uint32_t next_wake_up_delta_time_milliseconds,
                               int fd_count,
                               const int *fds )
{
    int r = -1;
    int n = 0;
    fd_set readable_set;
    struct timeval tv_timeout;
    int max_fd = -1;
    int i;

    if ( cur_time->tv_sec == 0 && cur_time->tv_usec == 0 )
    {
        gettimeofday( cur_time, 0 );
        us_net_timeout_add( wake_up_time, cur_time, next_wake_up_delta_time_milliseconds * 1000 );
    }

    us_net_timeout_calc( &tv_timeout, cur_time, wake_up_time );

    FD_ZERO( &readable_set );
    for ( i = 0; i < fd_count; ++i )
    {
        int fd = fds[i];
        if ( fd != -1 )
        {
            FD_SET( fd, &readable_set );
            if ( fd > max_fd )
            {
                max_fd = fd;
            }
        }
    }
    do
    {
        n = select( max_fd + 1, &readable_set, 0, 0, &tv_timeout );
    } while ( n < 0 && ( errno == EINTR ) );

    gettimeofday( cur_time, 0 );

    if ( n < 0 )
    {
        r = -2; /* Error */
    }
    else if ( n == 0 )
    {
        r = -1; /* Timeout */
    }
    else if ( n > 0 )
    {
        r = -1;
        for ( i = 0; i < max_fd + 1; i++ )
        {
            if ( FD_ISSET( i, &readable_set ) )
            {
                r = i;
                break;
            }
        }
    }
    if ( us_net_timeout_hit( cur_time, wake_up_time ) )
    {
        us_net_timeout_add( wake_up_time, wake_up_time, next_wake_up_delta_time_milliseconds );
    }

    return r;
}

void us_net_set_socket_nonblocking( int fd )
{
#if defined( US_CONFIG_POSIX )
    int val;
    int flags;
    val = fcntl( fd, F_GETFL, 0 );
    flags = O_NONBLOCK;
    val |= flags;
    fcntl( fd, F_SETFL, val );
#elif defined( WIN32 )
    u_long mode = 1;
    if ( ioctlsocket( fd, FIONBIO, &mode ) != NO_ERROR )
    {
        us_log_error( "fcntl F_SETFL O_NONBLOCK failed" );
    }
#endif
}

void us_net_set_socket_blocking( int fd )
{
#if defined( US_CONFIG_POSIX )
    int val;
    int flags;
    val = fcntl( fd, F_GETFL, 0 );
    flags = O_NONBLOCK;
    val &= ~flags;
    fcntl( fd, F_SETFL, val );
#elif defined( WIN32 )
    u_long mode = 0;
    if ( ioctlsocket( fd, FIONBIO, &mode ) != NO_ERROR )
    {
        us_log_error( "fcntl F_SETFL O_NONBLOCK failed" );
    }
#endif
}

#endif
