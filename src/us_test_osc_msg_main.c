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
#include "us_world.h"
#include "us_testutil.h"
#include "us_logger_printer.h"

#include "us_buffer.h"
#include "us_osc_msg.h"
#include "us_test_osc_msg_main.h"

#if US_ENABLE_PRINTING
#include "us_osc_msg_print.h"
#include "us_buffer_print.h"
#endif

/** \addtogroup us_osc_msg_test_msg */
/*@{*/

static bool do_test_osc_msg( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer );

static bool do_test_osc_msg_bundle( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer );

static bool test_unflatten( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer );

static bool test_unflatten( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer )
{
    bool r = false;
    us_log_probe();
    us_log_info( "unflattening message" );
    if ( us_osc_msg_is_msg_bundle( buffer ) )
    {
        us_osc_msg_bundle_t *bundle;
        us_log_info( "unflattening bundle" );
        bundle = us_osc_msg_bundle_unflatten( allocator, buffer, us_buffer_readable_count( buffer ), 0 );
        if ( bundle )
        {
            r = true;
#if US_ENABLE_PRINTING
            printer->printf( printer, "Parsed bundle: \n" );
            bundle->print( bundle, printer );
            printer->printf( printer, "\n" );
#endif
            {
                us_buffer_t *second_buffer = us_buffer_create( allocator, 4096 );
                if ( second_buffer )
                {
                    if ( bundle->flatten( bundle, second_buffer, 0 ) )
                    {
#if US_ENABLE_PRINTING
                        {
                            printer->printf( printer, "Flattened message buffer contents:\n" );
                            us_buffer_print( second_buffer, printer );
                        }
#endif
                    }
                }
            }
        }
    }
    else if ( us_osc_msg_is_msg( buffer ) )
    {
        us_osc_msg_t *msg;
        us_log_info( "unflattening message" );
        msg = us_osc_msg_unflatten( allocator, buffer, 0 );
        if ( msg )
        {
            r = true;
#if US_ENABLE_PRINTING
            printer->printf( printer, "Parsed message: \n" );
            msg->print( msg, printer );
            printer->printf( printer, "\n" );
#endif
            {
                us_buffer_t *second_buffer = us_buffer_create( allocator, 4096 );
                if ( second_buffer )
                {
                    if ( msg->flatten( msg, second_buffer, 0 ) )
                    {
#if US_ENABLE_PRINTING
                        {
                            printer->printf( printer, "Flattened message buffer contents:\n" );
                            us_buffer_print( second_buffer, printer );
                        }
#endif
                    }
                }
            }
        }
    }
    return r;
}

static bool do_test_osc_msg( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer )
{
    bool r = false;
    us_osc_msg_t *msg;
    us_log_probe();
    us_log_info( "creating test message" );
#if 0
    msg = us_osc_msg_form(
              allocator,
              "/in/1/level/db",
              ",f",
              -10.0f
          );
#endif
#if 0
    msg = us_osc_msg_form(
              allocator,
              "/out/1/filter/1/delay/ms",
              ",f",
              777.0f
          );
#endif
#if 1
    msg = us_osc_msg_form( allocator, "/out/1/gain/db", ",fs", 20.0f, "some string" );
#endif
    if ( msg )
    {
#if US_ENABLE_PRINTING
        us_osc_msg_print( msg, printer );
#endif
        if ( buffer )
        {
            us_log_debug( "flattening test message" );
            if ( msg->flatten( msg, buffer, 0 ) )
            {
#if US_ENABLE_PRINTING
                {
                    printer->printf( printer, "Flattened message buffer contents:\n" );
                    us_buffer_print( buffer, printer );
                }
#endif
                r = test_unflatten( allocator, buffer, printer );
            }
        }
    }
#if US_ENABLE_PRINTING
    printer->printf( printer, "\nResult: %d\n\n", r );
#endif
    return r;
}

static bool do_test_osc_msg_bundle( us_allocator_t *allocator, us_buffer_t *buffer, us_print_t *printer )
{
    bool r = false;
    us_osc_msg_bundle_t *bundle;
    us_log_probe();
    us_log_info( "creating test bundle" );
    bundle = us_osc_msg_bundle_create( allocator, 0, 0 );
    bundle->append( bundle, us_osc_msg_form( allocator, "/in/1/level/db", ",fs", -10.0f, "some string" ) );
    bundle->append( bundle, us_osc_msg_form( allocator, "/out/1/level/db", ",fs", 40.0, "another string" ) );
    if ( bundle )
    {
#if US_ENABLE_PRINTING
        us_osc_msg_bundle_print( bundle, printer );
#endif
        if ( buffer )
        {
            if ( bundle->flatten( bundle, buffer, 0 ) )
            {
#if US_ENABLE_PRINTING
                int32_t bundle_size = (int32_t)us_buffer_readable_count( buffer );
                {
                    printer->printf( printer, "bundle size: %ld\n", bundle_size );
                    printer->printf( printer, "Flattened message bundle buffer contents:\n" );
                    us_buffer_print( buffer, printer );
                }
#endif
                r = test_unflatten( allocator, buffer, printer );
            }
        }
    }
#if US_ENABLE_PRINTING
    printer->printf( printer, "\nResult: %d\n\n", r );
#endif
    return r;
}

int us_test_osc_msg_main( int argc, const char **argv )
{
    bool r = true;
    r = us_testutil_start( 8192, 8192, argc, argv );
    if ( r )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_info( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        {
            us_buffer_t *buffer = 0;
            us_log_debug( "resetting session allocator" );
            us_log_debug( "allocating 4k buffer from session" );
            buffer = us_buffer_create( us_testutil_session_allocator, 4096 );
            if ( buffer )
            {
                us_log_debug( "testing msg" );
                r &= do_test_osc_msg( us_testutil_session_allocator, buffer, us_stdout );
                buffer->destroy( buffer );
            }
            else
            {
                us_log_error( "error allocating buffer" );
            }
        }
        {
            us_buffer_t *buffer = 0;
            us_log_debug( "resetting session allocator" );
            us_simple_allocator_reset( &us_testutil_session_allocator_impl );
            us_log_debug( "allocating 4k buffer from session" );
            buffer = us_buffer_create( us_testutil_session_allocator, 4096 );
            if ( buffer )
            {
                us_log_debug( "testing bundle" );
                r &= do_test_osc_msg_bundle( us_testutil_session_allocator, buffer, us_stdout );
                buffer->destroy( buffer );
            }
            else
            {
                us_log_error( "error allocating buffer" );
            }
        }
        us_logger_finish();
        us_testutil_finish();
    }
    return r ? 0 : 1;
}

/*@}*/
