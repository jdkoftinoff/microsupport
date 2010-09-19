#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"

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
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/** \addtogroup us_test_buffer */
/*@{*/

bool us_test_buffer ( void );


bool us_test_buffer ( void )
{
    bool r = false;
    us_buffer_t *buf = us_buffer_create ( us_testutil_sys_allocator, 1024 );
    if ( buf )
    {
        if ( us_buffer_append_rounded_string ( buf, "Rounded String" ) )
        {
            if ( us_buffer_append_int32 ( buf, 0x12345678 ) )
            {
                if ( us_buffer_append_uint64 ( buf, 0x89abcdef, 0x01234567 ) )
                {
#if US_ENABLE_PRINTING
                    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "contents of test buffer:\n" );
                    buf->print ( buf, us_testutil_printer_stdout );
                    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "\n" );
#endif
                    r = true;
                }
            }
        }
    }
    else
    {
        us_log_error ( "expected to allocate 1024 bytes from allocator for buffer but failed" );
    }
    return r;
}

int main ( int argc, char **argv )
{
    int r = 1;
    if ( us_testutil_start ( 2048, 2048, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_buffer() )
            r = 0;
        us_log_info ( "Finishing us_test_buffer" );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}

/*@}*/


