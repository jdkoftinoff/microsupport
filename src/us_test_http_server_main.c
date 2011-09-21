#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_buffer_print.h"
#include "us_http_server.h"
#include "us_webapp.h"
#include "us_logger_printer.h"

#include "us_testutil.h"
#include "us_test_http_server_main.h"

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


/** \addtogroup us_test_http */
/*@{*/



static bool us_test_http_server ( void )
{
    bool r = true;
    us_allocator_t *allocator = us_testutil_sys_allocator;
    us_webapp_director_t director;
    us_webapp_diag_t *diag_app = (us_webapp_diag_t *)us_webapp_diag_create( allocator );
    if ( diag_app && us_webapp_director_init( &director, allocator ) )
    {
        us_http_server_handler_t *handler = (us_http_server_handler_t *)us_http_server_handler_create( allocator );
        us_webapp_director_add_404_app( &director, &diag_app->m_base );
        if ( handler )
        {
            if ( us_http_server_handler_init( &handler->m_base.m_base, allocator, 0, 0, 8192, &director ) )
            {
                us_buffer_t *data_to_server = &handler->m_base.m_incoming_queue;
                us_buffer_t *data_from_server = &handler->m_base.m_outgoing_queue;
                const char *request = "GET /some/path HTTP/1.1\r\nHost: localhost:80\r\nConnection: Close\r\n\r\n";
                handler->m_base.connected( &handler->m_base, 0, 0 );
                us_buffer_write( data_to_server, (uint8_t *)request, strlen( request));
                if ( handler->m_base.readable )
                {
                    handler->m_base.readable( &handler->m_base );
                }
                if ( handler->m_base.incoming_eof)
                {
                    handler->m_base.incoming_eof( &handler->m_base );
                }
                if ( handler->m_base.writable )
                {
                    handler->m_base.writable( &handler->m_base );
                }
                us_buffer_write_byte( data_from_server, '\0' );
            }
            handler->m_base.m_base.destroy( &handler->m_base.m_base );
            us_delete( allocator, handler );
            handler = (us_http_server_handler_t *)us_http_server_handler_create( allocator );
            if ( handler )
            {
                if ( us_http_server_handler_init( &handler->m_base.m_base, allocator, 0, 0, 8192, &director ) )
                {
                    us_buffer_t *data_to_server = &handler->m_base.m_incoming_queue;
                    us_buffer_t *data_from_server = &handler->m_base.m_outgoing_queue;
                    const char *request = "POST /some/post/path HTTP/1.1\r\nHost: localhost:80\r\nContent-Length: 40\r\nContent-Type: text/plain\r\nConnection: Close\r\n\r\n"
                                          "1234567890123456789012345678901234567890";
                    handler->m_base.connected( &handler->m_base, 0, 0 );
                    us_buffer_write( data_to_server, (uint8_t *)request, strlen( request));
                    if ( handler->m_base.readable )
                    {
                        handler->m_base.readable( &handler->m_base );
                    }
                    if ( handler->m_base.incoming_eof)
                    {
                        handler->m_base.incoming_eof( &handler->m_base );
                    }
                    if ( handler->m_base.writable )
                    {
                        handler->m_base.writable( &handler->m_base );
                    }
                    us_buffer_write_byte( data_from_server, '\0' );
                }
                handler->m_base.m_base.destroy( &handler->m_base.m_base );
                us_delete( allocator, handler );
            }
        }
        director.destroy( &director );
    }
    return r;
}

int us_test_http_server_main ( int argc, const char **argv )
{
    int r = 1;
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_TRACE );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_http_server() )
            r = 0;
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}


/*@}*/




