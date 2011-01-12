
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

#include "us_world.h"

#include "us_httpd.h"

us_reactor_handler_t *
us_http_server_reactor_handler_create(
        us_allocator_t *allocator
        )
{
    /* TODO: create our own structure */
    return us_reactor_handler_tcp_create( allocator );
}

bool us_http_server_reactor_handler_init(
        us_reactor_handler_t *self_,
        us_allocator_t *allocator,
        int fd,
        void *extra
        )
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r;
    r = us_reactor_handler_tcp_init (
            &self->m_base,
            allocator,
            fd,
            extra,
            8192,
            8192
        );
    self->readable = us_http_server_reactor_handler_readable;
    self->tick = us_http_server_reactor_handler_tick;
    self->connected = us_http_server_reactor_handler_connected;
    return r;
}

bool us_http_server_reactor_handler_connected (
    us_reactor_handler_tcp_t *self,
    struct sockaddr *addr,
    socklen_t addrlen
)
{
    bool r = false;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    if ( getnameinfo (
                addr,
                addrlen,
                host,
                sizeof ( host ),
                serv,
                sizeof ( serv ),
                NI_NUMERICHOST | NI_NUMERICSERV ) == 0
       )
    {
        us_log_info( "Accepting connection from '%s' port '%s'\n", host, serv );
        r = true;
    }
    return r;
}

bool us_http_server_reactor_handler_tick (
    us_reactor_handler_tcp_t *self_
)
{
    /* TODO:handle timeout to click lazy clients off */
    return true;
}

bool us_http_server_reactor_handler_readable(
    us_reactor_handler_tcp *self_
)
{
    /* TODO: read until full header of request is parsed,
       then dispatch to appropriate place
    */
    return false;
}
