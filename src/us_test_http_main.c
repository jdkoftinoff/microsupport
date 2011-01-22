#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_buffer_print.h"
#include "us_http.h"

#include "us_logger_printer.h"

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


/** \addtogroup us_test_http */
/*@{*/


static bool us_test_http ( void );

static bool us_test_http_request ( void );
static bool us_test_http_response ( void );

static bool us_test_http_request ( void )
{
    bool r = true;
    us_http_request_header_t *req;
    req = us_http_request_header_init_get (
              us_testutil_session_allocator,
              "www.meyersound.com:80",
              "/products/d-mitri/"
          );
    if ( req )
    {
        us_buffer_t *buf;
        buf = us_buffer_create ( us_testutil_session_allocator, 4096 );
        if ( buf )
        {
            r &= us_http_request_header_flatten ( req, buf );
            if ( r )
            {
                us_log_info ( "generated request:" );
                us_buffer_print_string ( buf, us_testutil_printer_stdout );
                {
                    us_http_request_header_t *parsed_req;
                    parsed_req = us_http_request_header_create( us_testutil_session_allocator );
                    if ( parsed_req )
                    {
                        if( us_http_request_header_parse ( parsed_req, buf ) )
                        {
                            us_log_info ( "parsed request:" );
                            us_buffer_reset ( buf );
                            r &= us_http_request_header_flatten ( parsed_req, buf );
                            us_buffer_print_string ( buf, us_testutil_printer_stdout );
                        }
                        parsed_req->destroy ( parsed_req );
                    }
                }
            }
            buf->destroy ( buf );
        }
        else
        {
            r = false;
        }
        req->destroy ( req );
    }
    else
    {
        r = 0;
    }
    return r;
}

static bool us_test_http_response ( void )
{
    bool r = true;
    const char *html = "<html><head><title>Test</title></head><body><p>Hello There</p></body></html>";
    us_http_response_header_t *resp;
    resp = us_http_response_header_create_ok (
               us_testutil_session_allocator,
               200,
               "text/html",
               ( uint32_t ) ( strlen ( html ) )
           );
    if ( resp )
    {
        us_buffer_t *buf;
        buf = us_buffer_create ( us_testutil_session_allocator, 4096 );
        if ( buf )
        {
            r &= us_http_response_header_flatten ( resp, buf );
            if ( r )
            {
                us_log_info ( "generated response:" );
                us_buffer_print_string ( buf, us_testutil_printer_stdout );
                us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "%s\n", html );
                {
                    us_http_response_header_t *parsed_resp;
                    parsed_resp = us_http_response_header_create( us_testutil_session_allocator );
                    if ( parsed_resp )
                    {
                        if( us_http_response_header_parse ( parsed_resp, buf ) )
                        {
                            us_log_info ( "parsed response:" );
                            us_buffer_reset ( buf );
                            r &= us_http_response_header_flatten ( parsed_resp, buf );
                            us_buffer_print_string ( buf, us_testutil_printer_stdout );
                        }
                        parsed_resp->destroy ( parsed_resp );
                    }
                }
            }
            buf->destroy ( buf );
        }
        else
        {
            r = false;
        }
    }
    else
    {
        r = 0;
    }
    return r;
}

static bool us_test_http ( void )
{
    bool r = true;
    if ( !us_test_http_request() )
    {
        r = false;
        us_log_error ( "http request test failed" );
    }
    if ( !us_test_http_response() )
    {
        r = false;
        us_log_error ( "http response test failed" );
    }
    return r;
}

int us_test_http_main ( int argc, char **argv )
{
    int r = 1;
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_http() )
            r = 0;
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}


/*@}*/




