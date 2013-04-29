#include "us_world.h"
#include "us_http_server.h"
#include "us_webapp.h"
#include "us_getopt.h"
#include "us_webapp_fs.h"
#include "us_logger_stdio.h"

#include "us_testutil.h"

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
 * Neither the name of J.D. Koftinoff Software, Ltd. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD..  BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/** \addtogroup us_test_net */
/*@{*/

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

typedef struct us_webapp_json_test_s
{
    us_webapp_t m_base;
} us_webapp_json_test_t;

us_webapp_t *us_webapp_json_test_create(
    us_allocator_t *allocator
);

void us_webapp_json_test_destroy(
    us_webapp_t *self
);

bool us_webapp_json_test_path_match(us_webapp_t *self, const char *path );

int us_webapp_json_test_dispatch(
    us_webapp_t *self,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
);

us_webapp_t *us_webapp_json_test_create(
    us_allocator_t *allocator
)
{
    us_webapp_json_test_t *self = us_new(allocator,us_webapp_json_test_t);
    if ( self )
    {
        self->m_base.destroy = us_webapp_json_test_destroy;
        self->m_base.path_match = us_webapp_json_test_path_match;
        self->m_base.dispatch = us_webapp_json_test_dispatch;
    }
    return &self->m_base;
}

void us_webapp_json_test_destroy(
    us_webapp_t *self_
)
{
    us_webapp_json_test_t *self = (us_webapp_json_test_t *)self_;
    us_webapp_destroy( &self->m_base );
}

bool us_webapp_json_test_path_match(us_webapp_t *self_, const char *path )
{
    static char p[] =  "/cgi-bin/json_test";
    if ( strncmp( path, p, sizeof(p) )==0 )
        return true;
    else
        return false;
}

int us_webapp_json_test_dispatch(
    us_webapp_t *self_,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    bool r=true;
    char json[4096];
    static int cnt=1;
    sprintf( json, "{ \"time\" : \%ld, \"access_count\" = %d }", (long)time(0), (int)cnt++ );
    r&=us_buffer_append_string( response_content, json );
    r&=us_http_response_header_init_ok( response_header, 200, "application/json", us_buffer_readable_count( response_content ),true);
    r&=us_http_response_header_set_no_cache( response_header );
    if ( r )
        return response_header->m_code;
    else
        return 500;
}

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
            256*1024,
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



bool us_example_http_server ( us_allocator_t *allocator )
{
    bool r = false;
    us_webapp_director_t director;
    us_reactor_t reactor;
    us_webapp_director_init( &director, allocator );
    us_webapp_director_add_app(&director, us_webapp_json_test_create(allocator));
    if ( !us_webapp_director_add_app(&director,us_webapp_redirect_create(allocator,"/","/index.html",302)) )
    {
        return false;
    }
    if ( !us_webapp_director_add_app(&director,us_webapp_fs_create(allocator, 0, "/", "." )) )
    {
        return false;
    }
    us_webapp_director_add_404_app( &director, 0 );
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
        while ( reactor.poll ( &reactor, 2000 ) && !us_platform_sigterm_seen && !us_platform_sigint_seen )
        {
            ;
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
    us_platform_init_sockets();
    us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
    r=us_example_http_server( &allocator.m_base );
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




