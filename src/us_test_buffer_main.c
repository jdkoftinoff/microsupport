#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_buffer_print.h"
#include "us_logger_printer.h"

#include "us_testutil.h"
#include "us_test_buffer_main.h"

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


/** \addtogroup us_test_buffer */
/*@{*/

static bool us_test_buffer ( void );

static us_buffer_t buffer;
static uint8_t mem[ 8 ];

static bool us_validate_buffer (
    us_buffer_t *self,
    uint16_t expected_writable_count,
    uint16_t expected_readable_count
);
static bool us_test_buffer ( void );

static bool us_validate_buffer (
    us_buffer_t *self,
    uint16_t expected_writable_count,
    uint16_t expected_readable_count
)
{
    bool r = true;
    uint16_t got_readable_count = us_buffer_readable_count ( self );
    uint16_t got_writable_count = us_buffer_writable_count ( self );
    bool expected_writable_byte = ( expected_writable_count > 0 );
    bool got_writable_byte = us_buffer_can_write_byte ( self );
    bool expected_readable_byte = ( expected_readable_count > 0 );
    bool got_readable_byte = us_buffer_can_read_byte ( self );
    int contig_writable = us_buffer_contig_writable_count( self );
    int contig_readable = us_buffer_contig_readable_count( self );
    if ( got_readable_byte != expected_readable_byte )
    {
        us_log_error ( "Expected readable byte: %d got: %d", expected_readable_byte, got_readable_byte );
        r = false;
    }
    if ( got_readable_count != expected_readable_count )
    {
        us_log_error ( "Expected readable count: %d got: %d", expected_readable_count, got_readable_count );
    }
    if ( got_writable_byte != expected_writable_byte )
    {
        us_log_error ( "Expected writable byte: %d got: %d", expected_writable_byte, got_writable_byte );
        r = false;
    }
    if ( got_writable_count != expected_writable_count )
    {
        us_log_error ( "Expected writable count: %d got: %d", expected_writable_count, got_writable_count );
    }
    us_log_debug( "Contig writable count is %d", contig_writable );
    us_log_error( "Contig readable count is %d", contig_readable );
    us_log_debug ( "buffer next_in=%d, next_out=%d", buffer.m_next_in, buffer.m_next_out );
    return r;
}

static bool us_test_buffer_queue ( void )
{
    bool r = true;
    uint16_t i;
    uint8_t v;
    uint8_t test_7_bytes[7] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22 };
    uint8_t read_7_bytes[7] = { 0, 0, 0, 0, 0, 0, 0 };
    us_log_debug ( "init buffer of 8 elements" );
    us_buffer_init ( &buffer, 0, mem, 8 );
    r &= us_validate_buffer ( &buffer, 7, 0 );
    us_log_debug ( "Try write 1 byte" );
    if ( us_buffer_can_write_byte ( &buffer ) )
    {
        us_buffer_write_byte ( &buffer, 0x01 );
    }
    r &= us_validate_buffer ( &buffer, 6, 1 );
    us_log_debug ( "Try write 1 byte" );
    if ( us_buffer_can_write_byte ( &buffer ) )
    {
        us_buffer_write_byte ( &buffer, 0x05 );
    }
    r &= us_validate_buffer ( &buffer, 5, 2 );
    us_log_debug ( "Try read first byte" );
    if ( us_buffer_can_read_byte ( &buffer ) )
    {
        v = us_buffer_read_byte ( &buffer );
        if ( v != 0x01 )
        {
            us_log_error ( "Expected 0x01, got 0x%02x", v );
            r = false;
        }
    }
    r &= us_validate_buffer ( &buffer, 6, 1 );
    us_log_debug ( "Try read second byte" );
    if ( us_buffer_can_read_byte ( &buffer ) )
    {
        v = us_buffer_read_byte ( &buffer );
        if ( v != 0x05 )
        {
            us_log_error ( "Expected 0x05, got 0x%02x", v );
            r = false;
        }
    }
    r &= us_validate_buffer ( &buffer, 7, 0 );
    us_log_debug ( "Try write 7 bytes" );
    if ( us_buffer_writable_count ( &buffer ) >= 7 )
    {
        us_buffer_write ( &buffer, test_7_bytes, sizeof ( test_7_bytes ) );
    }
    r &= us_validate_buffer ( &buffer, 0, 7 );
    us_log_debug ( "Try peek 7th byte" );
    v = us_buffer_peek ( &buffer, 6 );
    if ( v != test_7_bytes[6] )
    {
        us_log_error ( "Expected 0x%02x, got 0x%02x", test_7_bytes[6], v );
        r = false;
    }
    us_log_debug ( "Try skip 7 bytes" );
    us_buffer_skip ( &buffer, 7 );
    r &= us_validate_buffer ( &buffer, 7, 0 );
    us_log_debug ( "Try write 7 bytes" );
    if ( us_buffer_writable_count ( &buffer ) >= 7 )
    {
        us_buffer_write ( &buffer, test_7_bytes, sizeof ( test_7_bytes ) );
    }
    r &= us_validate_buffer ( &buffer, 0, 7 );
    us_log_debug ( "Try read 7 bytes" );
    us_buffer_read ( &buffer, read_7_bytes, sizeof ( read_7_bytes ) );
    for ( i = 0; i < sizeof ( read_7_bytes ); ++i )
    {
        if ( test_7_bytes[i] != read_7_bytes[i] )
        {
            us_log_error ( "Expected byte %d to be 0x%02x, got 0x%02x", i, test_7_bytes[i], read_7_bytes[i] );
            r = false;
        }
    }
    r &= us_validate_buffer ( &buffer, 7, 0 );
    return r;
}


static bool us_test_buffer ( void )
{
    bool r = true;
    int repeat_count=10;
    int big_loop=100;
    us_buffer_t *buf = us_buffer_create ( us_testutil_sys_allocator, 500 );
    if ( buf )
    {
        int i;
        int j;
        for ( j=0; j<big_loop; ++j )
        {
            for ( i=0; i<repeat_count; ++i )
            {
                bool r1=false;
                if ( us_buffer_append_rounded_string ( buf, "Rounded String" ) )
                {
                    if ( us_buffer_append_int32 ( buf, 0x12345678 ) )
                    {
                        if ( us_buffer_append_uint64 ( buf, 0x89abcdef, 0x01234567 ) )
                        {
                            r1 = true;
                        }
                    }
                }
                if ( !r1 )
                {
                    us_log_error( "unable to append items" );
                }
                r&=r1;
                if ( !r )
                    break;
            }
            for ( i=0; i<repeat_count; ++i )
            {
                bool r1=false;
                {
                    char rs[128];
                    if ( us_buffer_read_rounded_string( buf, rs, sizeof(rs) ) )
                    {
                        if ( strcmp(rs,"Rounded String")==0 )
                        {
                            int32_t v;
                            if ( us_buffer_read_int32( buf, &v ) )
                            {
                                if ( v==0x12345678 )
                                {
                                    uint32_t a,b;
                                    if ( us_buffer_read_uint64( buf, &a, &b ) )
                                    {
                                        if ( a==0x89abcdef && b==0x01234567 )
                                        {
                                            r1=true;
                                        }
                                        else
                                        {
                                            us_log_error( "unable to validate uint64" );
                                        }
                                    }
                                    else
                                    {
                                        us_log_error( "unable to read uint64" );
                                    }
                                }
                                else
                                {
                                    us_log_error( "unable to validate int32" );
                                }
                            }
                            else
                            {
                                us_log_error( "unable to read int32" );
                            }
                        }
                        else
                        {
                            us_log_error( "unable to validate rounded string" );
                        }
                    }
                    else
                    {
                        us_log_error( "unable to read rounded string" );
                    }
                }
                r&=r1;
                if ( !r )
                {
#if US_ENABLE_PRINTING
                    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "contents of test buffer:\n" );
                    us_buffer_print ( buf, us_testutil_printer_stdout );
                    us_testutil_printer_stdout->printf ( us_testutil_printer_stdout, "\n" );
#endif
                    break;
                }
            }
        }
    }
    else
    {
        us_log_error ( "expected to allocate 1024 bytes from allocator for buffer but failed" );
    }
    r&=us_test_buffer_queue();
    return r;
}

int us_test_buffer_main ( int argc, const char **argv )
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


