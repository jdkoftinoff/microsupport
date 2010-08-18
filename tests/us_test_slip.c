#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_slip.h"

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


/** \addtogroup us_test_slip */
/*@{*/


bool us_test_slip ( void );
us_buffer_t * us_test_slip_fill_buffer ( void );
void us_test_slip_callback ( us_slip_decoder_t *self, us_buffer_t *buf );


us_buffer_t * us_test_slip_fill_buffer ( void )
{
    bool r = false;
    us_buffer_t *buf = us_buffer_create ( us_testutil_sys_allocator, 1024 );
    
    if ( buf )
    {
        if ( us_buffer_append_rounded_string ( buf, "Rounded String" ) )
        {
            if ( us_buffer_append_int32 ( buf, 0x1234c078 ) ) /* note embedded C0 = slip END */
            {
                if ( us_buffer_append_uint64 ( buf, 0x89abcdef, 0x012345db ) ) /* note embedded DB = slip ESC */
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
    
    if ( !r )
        buf = 0;
        
    return buf;
}

void us_test_slip_callback ( us_slip_decoder_t *self, us_buffer_t *buf )
{
    ( void ) self;
    us_log_info ( "Successfully parsed slip encoded buffer" );
#if US_ENABLE_PRINTING
    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "contents of decoded buffer:\n" );
    buf->print ( buf, us_testutil_printer_stdout );
    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "\n" );
#endif
}

bool us_test_slip ( void )
{
    bool r = false;
    us_buffer_t *src_buf = us_test_slip_fill_buffer();
    us_buffer_t *slipped_buf = us_buffer_create ( us_testutil_sys_allocator, 1024 );
    us_slip_decoder_t *slip_decoder = us_slip_decoder_create (
                                          us_testutil_sys_allocator,
                                          1024,
                                          us_test_slip_callback
                                      );
                                      
    if ( src_buf && slipped_buf && slip_decoder )
    {
        us_log_info ( "encoding buffer" );
        
        if ( us_slip_encode ( slipped_buf, src_buf ) )
        {
#if US_ENABLE_PRINTING
            us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "contents of encoded buffer:\n" );
            slipped_buf->print ( slipped_buf, us_testutil_printer_stdout );
            us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "\n" );
#endif
            us_log_info ( "decoding buffer" );
            
            if ( us_slip_decoder_parse_buffer ( slip_decoder, slipped_buf ) == 1 )
            {
                r = true;
            }
        }
    }
    
    else
    {
        us_log_error ( "Error allocating buffers or slip decoder" );
    }
    
    return r;
}

int main ( int argc, char **argv )
{
    int r = 1;
    
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        
        if ( us_test_slip() )
            r = 0;
            
        us_log_info ( "Finishing us_test_slip" );
        us_logger_finish();
        us_testutil_finish();
    }
    
    return r;
}


/*@}*/




