#include "us_world.h"

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
 * Neither the name of Meyer Sound Laboratories, Inc. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC.  BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_allocator.h"
#include "us_buffer.h"
#include "us_net.h"
#include "us_logger_printer.h"

#include "us_testutil.h"

#include "us_reactor.h"
#include <ctype.h>

#ifndef US_EXAMPLE_HTTP_HOST
#define US_EXAMPLE_HTTP_HOST "github.com"
#endif

/** \ingroup examples */
/** \defgroup example_reactor */
/*@{*/

bool global_quit = false;

bool us_example_reactor ( void );

us_reactor_handler_t * us_example_reactor_handler_http_create ( void );

bool us_example_reactor_handler_http_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
);

bool us_example_reactor_handler_http_tick (
    us_reactor_handler_tcp_t *self
);

bool us_example_reactor_handler_http_readable (
    us_reactor_handler_tcp_t *self
);

void us_example_reactor_handler_http_closed (
    us_reactor_handler_tcp_t *self
);


us_reactor_handler_t * us_example_reactor_handler_echo_create ( void );

bool us_example_reactor_handler_echo_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
);

bool us_example_reactor_handler_echo_connected (
    us_reactor_handler_tcp_t *self,
    struct sockaddr *addr,
    socklen_t addrlen
);

bool us_example_reactor_handler_echo_tick (
    us_reactor_handler_tcp_t *self
);

bool us_example_reactor_handler_echo_readable (
    us_reactor_handler_tcp_t *self
);


us_reactor_handler_t * us_example_reactor_handler_quitter_create ( void );

bool us_example_reactor_handler_quitter_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
);

bool us_example_reactor_handler_quitter_readable (
    us_reactor_handler_tcp_t *self
);


us_reactor_handler_t * us_example_reactor_handler_http_create ( void )
{
    return us_reactor_handler_tcp_create();
}

bool us_example_reactor_handler_http_init (
    us_reactor_handler_t *self_,
    int fd,
    void *extra
)
{
    us_reactor_handler_tcp_t *self = (us_reactor_handler_tcp_t *)self_;
    bool r = us_reactor_handler_tcp_init(self_, fd, extra, 8192, 2048);

    if( r )
    {
        static const char *req = "GET / HTTP/1.0\r\nHost: "US_EXAMPLE_HTTP_HOST"\r\n\r\n";
        self->readable = us_example_reactor_handler_http_readable;
        self->tick = us_example_reactor_handler_http_tick;
        self->closed = us_example_reactor_handler_http_closed;

        us_queue_write(&self->outgoing_queue, (const uint8_t*)req, strlen(req) );
    }

    return r;
}

void us_example_reactor_handler_http_closed (
    us_reactor_handler_tcp_t *self
)
{
    FILE *f = (FILE *)self->base.extra;
    fprintf( f, "%s\n", __FUNCTION__ );
}


bool us_example_reactor_handler_http_tick (
    us_reactor_handler_tcp_t *self
)
{
    FILE *f = (FILE *)self->base.extra;
    fprintf( f, "%s\n", __FUNCTION__ );
    return true;
}

bool us_example_reactor_handler_http_readable (
    us_reactor_handler_tcp_t *self
)
{
    FILE *f = (FILE *) self->base.extra;
    fprintf( f, "HTTP Response data (len=%d):\n", us_queue_readable_count(&self->incoming_queue) );

    while ( us_queue_can_read_byte ( &self->incoming_queue ) )
    {
        char c = ( char ) us_queue_read_byte ( &self->incoming_queue );
        fprintf( f, "%c", c );
    }

    return true;
}






bool us_example_reactor_handler_echo_readable (
    us_reactor_handler_tcp_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    /* echo all incoming data from input queue to output queue while converting to uppercase */
    while ( us_queue_can_write_byte ( &self->outgoing_queue ) &&
            us_queue_can_read_byte ( &self->incoming_queue ) )
    {
        uint8_t c = us_queue_read_byte ( &self->incoming_queue );
        c = toupper ( c );
        us_queue_write_byte ( &self->outgoing_queue, c );
    }
    return true;
}

us_reactor_handler_t *
us_example_reactor_handler_echo_create ( void )
{
    return us_reactor_handler_tcp_create();
}

bool us_example_reactor_handler_echo_init (
    us_reactor_handler_t *self_,
    int fd,
    void *extra
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r;
    r = us_reactor_handler_tcp_init (
            &self->base,
            fd,
            extra,
            16384,
            1500
        );
    self->readable = us_example_reactor_handler_echo_readable;
    self->tick = 0;
    return r;
}


bool us_example_reactor_echo_server_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
)
{
    return us_reactor_handler_tcp_server_init (
               self,
               fd,
               extra,
               us_example_reactor_handler_echo_create,
               us_example_reactor_handler_echo_init
           );
}


bool us_example_reactor_quitter_server_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
)
{
    return us_reactor_handler_tcp_server_init (
               self,
               fd,
               extra,
               us_example_reactor_handler_quitter_create,
               us_example_reactor_handler_quitter_init
           );
}


us_reactor_handler_t *
us_example_reactor_handler_quitter_create ( void )
{
    return us_reactor_handler_tcp_create();
}

bool us_example_reactor_handler_quitter_init (
    us_reactor_handler_t *self_,
    int fd,
    void *extra
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r;
    r = us_reactor_handler_tcp_init (
            &self->base,
            fd,
            extra,
            2048,
            1500
        );
    self->readable = us_example_reactor_handler_quitter_readable;
    self->tick = 0;
    return r;
}

bool us_example_reactor_handler_echo_connected (
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
        fprintf ( stdout, "Accepting connection from '%s' port '%s'\n", host, serv );
        r = true;
    }
    return r;
}


bool us_example_reactor_handler_echo_tick (
    us_reactor_handler_tcp_t *self
)
{
    return true;
}

bool us_example_reactor_handler_quitter_readable (
    us_reactor_handler_tcp_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    /* wait for the letter Q, then signal a quit flag */
    while ( us_queue_can_read_byte ( &self->incoming_queue ) )
    {
        char c = ( char ) us_queue_read_byte ( &self->incoming_queue );
        if ( c == 'Q' )
        {
            global_quit = true;
        }
    }
    return true;
}


bool us_example_reactor ( void )
{
    bool r = false;
    us_reactor_t reactor;
    r = us_reactor_init (
            &reactor,
            16 /* max simultaneous sockets, including server sockets and connections */
        );
    if ( r )
    {
        r = us_reactor_create_tcp_client(
            &reactor,
            US_EXAMPLE_HTTP_HOST, "80",
            (void *)stdout,
            us_example_reactor_handler_http_create,
            us_example_reactor_handler_http_init
            );
    }
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                0, "8989",
                SOCK_STREAM,
                0,
                us_reactor_handler_tcp_server_create,
                us_example_reactor_echo_server_init
            );
    }
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                0, "8988",
                SOCK_STREAM,
                0,
                us_reactor_handler_tcp_server_create,
                us_example_reactor_quitter_server_init
            );
    }
    if ( r )
    {
        while ( !global_quit && reactor.poll ( &reactor, 2000 ) )
        {
            fprintf ( stdout, "tick\n" );
        }
        reactor.destroy ( &reactor );
        r = true;
    }
    return r;
}

int main ( int argc, char **argv )
{
    bool r = us_example_reactor();
    if ( r )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*@}*/
