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

#include "us_world.h"
#include "us_midi.h"
#include "us_mtc.h"
#include "us_mtc_parser.h"
#include "us_buffer.h"

#include "us_logger_printer.h"

#include "us_testutil.h"
#include "us_test_mtc_main.h"


/** \addtogroup us_test_midi */
/*@{*/

static void print_msg ( us_midi_msg_t *m );
static void print_sysex ( us_midi_sysex_t *ex );
static bool us_test_mtc_parser ( const uint8_t *input_data, int32_t input_data_len );

static uint8_t test_data_1[] =
{
    0xf8,
    0x91,
    0x40, 0x7f, 0x44, 0x7f, 0x47, 0x7f,
    0xf8,
    0x40, 0x00, 0x44, 0x00, 0x47, 0x00,
    0xf8,
    0xf0, 0x1f, 0x7e, 0x7f, 0x7f, 0x01, 0xf7,

    0xf1, 0x00,
    0xf1, 0x10,
    0xf1, 0x23,
    0xf1, 0x32,
    0xf1, 0x45,
    0xf1, 0x54,
    0xf1, 0x61,
    0xf1, 0x70,

    0xf1, 0x01,
    0xf1, 0x10,
    0xf1, 0x23,
    0xf1, 0x32,
    0xf1, 0x45,
    0xf1, 0x54,
    0xf1, 0x61,
    0xf1, 0x70,

    0xf1, 0x02,
    0xf1, 0x10,
    0xf1, 0x23,
    0xf1, 0x32,
    0xf1, 0x45,
    0xf1, 0x54,
    0xf1, 0x61,
    0xf1, 0x70,
    0xfe
};

static void print_sysex ( us_midi_sysex_t *ex )
{
    int i;
    int l = us_midi_sysex_get_length ( ex );
    us_log_info ( "Sysex Len=%d", l );
    for ( i = 0; i < l; ++i )
    {
        us_log_info ( "%02x ", ( int ) us_midi_sysex_get_data ( ex, i ) );
    }
}


static void print_msg ( us_midi_msg_t *m )
{
    char buf[129];
    int l = us_midi_msg_get_length ( m );
    us_log_info ( "Msg : " );
    if ( l == 1 )
    {
        us_log_info ( " %02x \t=", us_midi_msg_get_status ( m ) );
    }
    else if ( l == 2 )
    {
        us_log_info ( " %02x %02x \t=", us_midi_msg_get_status ( m ), us_midi_msg_get_byte1 ( m ) );
    }
    else if ( l == 3 )
    {
        us_log_info ( " %02x %02x %02x \t=", us_midi_msg_get_status ( m ), us_midi_msg_get_byte1 ( m ), us_midi_msg_get_byte2 ( m ) );
    }
    us_midi_msg_to_text ( m, buf, sizeof ( buf ) );
    us_log_info ( "%s", buf );
}


static bool us_test_mtc_parser ( const uint8_t *input_data, int32_t input_data_len )
{
    bool r = false;
    const uint8_t *input_ptr = input_data;
    int32_t input_todo = input_data_len;
    us_midi_parser_t *p;
    us_mtc_parser_t mtc_parser;
    us_mtc_parser_init( &mtc_parser );
    p = us_midi_parser_create ( us_testutil_session_allocator, 256 );
    if ( p )
    {
        us_midi_msg_t m;
        us_mtc_t t;
        us_mtc_init(&t);
        bool t_valid=false;
        struct timeval cur_time;
        while ( input_todo > 0 )
        {
            us_gettimeofday( &cur_time );
            if ( us_midi_parser_parse ( p, *input_ptr, &m ) )
            {
                if ( us_midi_msg_is_sysex ( &m ) )
                {
                    print_sysex ( us_midi_parser_get_sysex ( p ) );
                }
                else
                {
                    print_msg ( &m );
                }
                t_valid = us_mtc_parser_parse( &mtc_parser, &cur_time, &m, us_midi_parser_get_sysex(p), &t );
                if( t_valid )
                {
                    char buf[128];
                    if( us_mtc_print( &t, true, buf, sizeof(buf) ) )
                    {
                        us_log_info( "MTC Parsed: %s", buf );
                    }
                }
            }
            input_ptr++;
            input_todo--;
        }
        r = true;
        p->destroy ( p );
    }
    return r;
}

int us_test_mtc_main ( int argc, const char **argv )
{
    int r = 1;
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_mtc_parser ( test_data_1, sizeof ( test_data_1 ) ) )
            r = 0;
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}


/*@}*/
