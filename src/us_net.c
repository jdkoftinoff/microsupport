#include "us_world.h"
#include "us_net.h"

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

#if US_ENABLE_BSD_SOCKETS

struct addrinfo *us_net_get_addrinfo (
    const char *ip_addr,
    const char *ip_port,
    int type,
    bool for_server
)
{
    int e;
    struct addrinfo *ai;
    struct addrinfo hints;
    memset ( &hints, '\0', sizeof ( hints ) );

#if defined(AI_NUMERICHOST)
    hints.ai_flags |= AI_NUMERICHOST;
#endif

#if defined(AI_NUMERICSERV)
    hints.ai_flags |= AI_NUMERICSERV;
#endif

    if ( for_server )
        hints.ai_flags |= AI_PASSIVE;

    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = 0;

    if( ip_addr!=0 && *ip_addr=='\0' )
        ip_addr=0;
    if( ip_port!=0 && *ip_port=='\0' )
        ip_port=0;

    do
    {
        e = getaddrinfo ( ip_addr, ip_port, &hints, &ai );
    }
    while ( e == EAI_AGAIN );

    if ( e != 0 )
    {
        fprintf ( stderr, "getaddrinfo: %s\n", gai_strerror ( e ) );
        ai = 0;
    }

    return ai;
}

bool us_net_get_nameinfo (
    struct addrinfo *ai,
    char *hostname_buf,
    int hostname_buf_len,
    char *serv_buf,
    int serv_buf_len
)
{
    bool r = false;

    if ( getnameinfo (
                ai->ai_addr, ai->ai_addrlen,
                hostname_buf,
                hostname_buf_len,
                serv_buf,
                serv_buf_len,
                NI_NUMERICHOST | NI_NUMERICSERV
            ) == 0 )
    {
        r = true;
    }
    return r;
}


int us_net_create_udp_socket (
    struct addrinfo *ai,
    bool do_bind
)
{
    int r = -1;
    int s = -1;
    bool broadcast=false;
    if ( ai )
    {
        s = socket ( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
        if ( s >= 0 )
        {
            int on = 1;
            r = s;
            if ( broadcast )
            {
                if ( setsockopt ( s, SOL_SOCKET, SO_BROADCAST, ( const char * ) &on, sizeof ( on ) ) == -1 )
                {
                    perror ( "setsockopt SO_BROADCAST:" );
                    abort();
                }
            }
            if ( do_bind )
            {
                if ( setsockopt ( s, SOL_SOCKET, SO_REUSEADDR, ( const char * ) &on, sizeof ( on ) ) == -1 )
                {
                    perror ( "setsockopt SO_REUSEADDR:" );
                    abort();
                }
                if ( bind ( s, ai->ai_addr, ai->ai_addrlen ) == 0 )
                {
                    r = s;
                }
                else
                {
                    perror ( "socket: " );
                }
            }
        }
        else
        {
            perror ( "socket: " );
        }
        return r;
    }
    if ( r == -1 && s != -1 )
    {
        closesocket ( s );
    }
    return s;
}

int us_net_create_multicast_rx_udp_socket (
    struct addrinfo *listenaddr,
    struct addrinfo *multicastgroup,
    const char *interface_name
)
{
    int s = -1;
    int if_index = 0;
    if ( interface_name && *interface_name != '\0' )
        if_index = if_nametoindex ( interface_name );
    if ( listenaddr == 0 )
    {
        if ( multicastgroup->ai_family == PF_INET6 )
        {
            listenaddr = us_net_get_addrinfo ( "0::0", 0, SOCK_DGRAM, true );
            if ( listenaddr )
            {
                ( ( struct sockaddr_in6 * ) listenaddr->ai_addr )->sin6_port = ( ( struct sockaddr_in6 * ) multicastgroup->ai_addr )->sin6_port;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            listenaddr = us_net_get_addrinfo ( "0.0.0.0", 0, SOCK_DGRAM, true );
            if ( listenaddr )
            {
                ( ( struct sockaddr_in * ) listenaddr->ai_addr )->sin_port = ( ( struct sockaddr_in * ) multicastgroup->ai_addr )->sin_port;
            }
            else
            {
                return -1;
            }
        }
    }
    s = socket ( listenaddr->ai_family, listenaddr->ai_socktype, listenaddr->ai_protocol );
    if ( s < 0 )
    {
        perror ( "socket:" );
        abort();
    }
    {
        int on = 1;
        if ( setsockopt ( s, SOL_SOCKET, SO_REUSEADDR, ( const char * ) &on, sizeof ( on ) ) == -1 )
        {
            perror ( "setsockopt SO_REUSEADDR:" );
            abort();
        }
    }
    if ( bind ( s, listenaddr->ai_addr, listenaddr->ai_addrlen ) != 0 )
    {
        closesocket ( s );
        perror ( "bind: " );
        abort();
    }
    if ( multicastgroup->ai_family == PF_INET6 )
    {
        struct ipv6_mreq multicast_request;
        multicast_request.ipv6mr_multiaddr = ( ( struct sockaddr_in6* ) multicastgroup->ai_addr )->sin6_addr;
        multicast_request.ipv6mr_interface = if_index;
        if ( setsockopt (
                    s,
                    IPPROTO_IPV6,
                    IPV6_JOIN_GROUP,
                    ( char* ) &multicast_request,
                    sizeof ( multicast_request ) ) != 0
           )
        {
            perror ( "setsockopt IPV6_JOIN_GROUP:" );
            closesocket ( s );
            abort();
        }
    }
    else
    {
        if ( multicastgroup->ai_family == PF_INET )
        {
            struct in_addr in_local;
            in_local.s_addr = ( ( struct sockaddr_in * ) listenaddr->ai_addr )->sin_addr.s_addr;
            struct ip_mreq multicast_request;
            memcpy ( &multicast_request.imr_multiaddr,
                     & ( ( struct sockaddr_in* ) ( multicastgroup->ai_addr ) )->sin_addr,
                     sizeof ( multicast_request.imr_multiaddr ) );
            multicast_request.imr_interface.s_addr = in_local.s_addr;
            if ( setsockopt (
                        s,
                        IPPROTO_IP,
                        IP_ADD_MEMBERSHIP,
                        ( char* ) &multicast_request, sizeof ( multicast_request ) ) != 0
               )
            {
                perror ( "setsockopt IP_ADD_MEMBERSHIP:" );
                closesocket ( s );
                abort();
            }
        }
    }
    return s;
}


int us_net_create_multicast_tx_udp_socket (
    struct addrinfo *localaddr,
    struct addrinfo *multicastgroup,
    const char *interface_name
)
{
    int s = -1;
    int if_index = 0;
    if ( interface_name && *interface_name != '\0' )
        if_index = if_nametoindex ( interface_name );
    /* TODO: use if_index to bind socket to transmit only on interface */
    if ( !localaddr )
    {
        if ( multicastgroup->ai_family == PF_INET6 )
            localaddr = us_net_get_addrinfo ( "0::0", 0, SOCK_DGRAM, false );
        else
            localaddr = us_net_get_addrinfo ( "0.0.0.0", 0, SOCK_DGRAM, false );
    }
    s = socket ( localaddr->ai_family, localaddr->ai_socktype, localaddr->ai_protocol );
    if ( s < 0 )
    {
        perror ( "socket:" );
        abort();
    }
    if ( localaddr->ai_family == PF_INET )
    {
        struct in_addr in_local;
        in_local.s_addr = ( ( struct sockaddr_in * ) localaddr->ai_addr )->sin_addr.s_addr;
        if ( setsockopt (
                    s,
                    IPPROTO_IP,
                    IP_MULTICAST_IF,
                    ( char * ) &in_local,
                    sizeof ( in_local )
                ) != 0 )
        {
            perror ( "setsockopt IP_MULTICAST_IF" );
            closesocket ( s );
            abort();
        }
    }
    return s;
}

int us_net_create_tcp_socket (
    struct addrinfo *ai,
    bool do_bind
)
{
    int r = -1;
    int s = -1;
    if ( ai )
    {
        s = socket ( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
        if ( s >= 0 )
        {
            r = s;
            if ( do_bind )
            {
                if ( bind ( s, ai->ai_addr, ai->ai_addrlen ) == 0 )
                {
                    r = s;
                }
                else
                {
                    perror ( "socket: " );
                }
            }
        }
        else
        {
            perror ( "socket: " );
        }
        return r;
    }
    if ( r == -1 && s != -1 )
    {
        closesocket ( s );
    }
    return s;
}



void  us_net_timeout_add ( struct timeval *result, struct timeval *cur_time, uint32_t microseconds_to_add )
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

bool  us_net_timeout_calc ( struct timeval *result, struct timeval *cur_time, struct timeval *next_time )
{
    bool r = false;
    if ( us_net_timeout_hit ( cur_time, next_time ) )
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

bool us_net_timeout_hit ( struct timeval *cur_time, struct timeval *next_time )
{
    bool r = false;
    if ( cur_time->tv_sec > next_time->tv_sec ||
            ( cur_time->tv_sec == next_time->tv_sec && cur_time->tv_usec >= next_time->tv_usec ) )
    {
        r = true;
    }
    return r;
}

bool
us_net_blocking_send(
    int sock,
    const void *data_,
    int32_t len
)
{
    int32_t todo=len;
    const uint8_t *data = (const uint8_t *)data_;
    int cnt;
    while (todo>0)
    {
        do
        {
            cnt=send(sock, data, todo, 0);
        }
        while(cnt<0 && errno==EINTR);
        if( cnt<=0)
            break;
        todo-=cnt;
        data+=cnt;
    }
    return (todo==0);
}


#endif

