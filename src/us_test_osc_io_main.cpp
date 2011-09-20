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
#include "us_world.hpp"
#include "us_testutil.hpp"
#include "us_logger_printer.hpp"

#include "us_buffer.hpp"
#include "us_osc_msg.hpp"
#include "us_test_osc_io_main.hpp"

#if US_ENABLE_PRINTING
#include "us_osc_msg_print.hpp"
#include "us_buffer_print.hpp"
#endif

/** \addtogroup us_osc_msg_test_msg */
/*@{*/

static bool
do_test_osc_io(
    us_allocator_t *allocator,
    us_buffer_t *buffer,
    us_print_t *printer
);


static bool
do_test_osc_io(
    us_allocator_t *US_UNUSED(allocator),
    us_buffer_t *US_UNUSED(buffer),
    us_print_t *US_UNUSED(printer)
)
{
    /* TODO */
    return false;
}



int us_test_osc_io_main( int argc, const char **argv )
{
    bool r=true;
    r=us_testutil_start(8192,8192,argc,argv);
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
            buffer = us_buffer_create(us_testutil_session_allocator, 4096);
            if ( buffer )
            {
                us_log_debug( "testing msg" );
                r &= do_test_osc_io(
                         us_testutil_session_allocator,
                         buffer,
                         us_stdout
                     );
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
    return r ? 0:1;
}

/*@}*/

