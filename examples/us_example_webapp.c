#include "us_world.h"
#include "us_http_server.h"
#include "us_getopt.h"
#include "us_logger_stdio.h"
#include "us_webapp.h"
#include "us_testutil.h"

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


/** \addtogroup us_test_net */
/*@{*/

bool global_quit = false;

bool us_example_http_server ( us_allocator_t *allocator );

bool us_example_http_server_handler_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra
);

bool us_example_http_server_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
);

us_reactor_handler_t * us_example_quitter_create ( us_allocator_t *allocator );

bool us_example_quitter_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
);

bool us_example_quitter_readable (
    us_reactor_handler_tcp_t *self
);


bool us_example_http_server_handler_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    bool r;
    r = us_http_server_handler_init(
            self_,
            allocator,
            fd,
            extra,
            16384,
            (us_webapp_director_t *)extra
        );
    return r;
}

bool us_example_http_server_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    return us_reactor_handler_tcp_server_init (
               self,
               allocator,
               fd,
               extra,
               us_http_server_handler_create,
               us_example_http_server_handler_init
           );
}



bool us_example_reactor_quitter_server_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    return us_reactor_handler_tcp_server_init (
               self,
               allocator,
               fd,
               extra,
               us_example_quitter_create,
               us_example_quitter_init
           );
}


us_reactor_handler_t *
us_example_quitter_create ( us_allocator_t *allocator )
{
    return us_reactor_handler_tcp_create( allocator );
}

bool us_example_quitter_init (
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
            1024,
            256
        );
    self->readable = us_example_quitter_readable;
    self->tick = 0;
    return r;
}

bool us_example_quitter_readable (
    us_reactor_handler_tcp_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    /* wait for the letter Q, then signal a quit flag */
    while ( us_buffer_can_read_byte ( &self->m_incoming_queue ) )
    {
        char c = ( char ) us_buffer_read_byte ( &self->m_incoming_queue );
        if ( c == 'Q' )
        {
            global_quit = true;
        }
    }
    return true;
}


bool us_example_webapp ( us_allocator_t *allocator )
{
    bool r = false;
    us_webapp_director_t director;
    us_webapp_director_init( &director, allocator );
    us_reactor_t reactor;
    r = us_reactor_init (
            &reactor,
            allocator,
            16 /* max simultaneous sockets, including server sockets and connections */
        );
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                allocator,
                0, "8080",
                SOCK_STREAM,
                (void *)&director,
                us_reactor_handler_tcp_server_create,
                us_example_http_server_init
            );
    }
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                allocator,
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
    director.destroy( &director );
    return r;
}

int main ( int argc, char **argv )
{
    bool r;
    us_malloc_allocator_t allocator;
    us_malloc_allocator_init( &allocator );
#if US_ENABLE_LOGGING
    us_logger_stdio_start ( stdout, stderr );
#endif
    us_log_set_level ( US_LOG_LEVEL_TRACE );
    us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
    r=us_example_webapp( &allocator.m_base );
    us_malloc_allocator_destroy( &allocator.m_base );
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




