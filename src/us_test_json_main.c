#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_buffer_print.h"
#include "us_json.h"

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


/** \addtogroup us_test_json */
/*@{*/


static bool us_test_json ( void );

static bool us_test_json ( void )
{
    bool r = false;
    us_allocator_t *allocator = us_testutil_sys_allocator;
    us_json_t *j = us_json_create( allocator );
    us_buffer_t *b = us_buffer_create( allocator, 1024 );
    if( !b )
    {
        abort();
    }
    if( j )
    {
        int i;
        us_json_t *a=0;
        us_json_t *a1=0;
        char name[32]="Jeff";
        char city[32]="Vernon";
        char country[32]="Canada";
        int32_t histogram[11] = { 1, 3,6,10, 15, 14, 11, 4, 2, 1, 0 };
        static int32_t hitpoints=90210;
        r=true;
        if( !us_json_append_string_ptr( j, "name", name ) )
            r=false;
        if( !us_json_append_int32_ptr( j, "hitpoints", &hitpoints ) )
            r=false;
        a=us_json_append_object( j, "address" );
        if( !us_json_append_string_ptr( a, "city", city ) )
            r=false;
        if( !us_json_append_string_ptr( a, "country", country ) )
            r=false;
        a1=us_json_append_object( j, "histogram" );
        us_json_set_array( a1, true );
        for( i=0; i<sizeof(histogram)/sizeof(histogram[0]); ++i )
        {
            if( !us_json_append_int32_ptr( a1, 0, &histogram[i] ))
            {
                r=false;
                break;
            }
        }
        r&=us_json_flatten_to_buffer( j, b );
        r&=us_buffer_append_string( b, ", \n" );
        strcpy( name, "John" );
        strcpy( city, "Berkeley" );
        strcpy( country, "United States" );
        hitpoints = 999;
        for( i=0; i<sizeof(histogram)/sizeof(histogram[0]); ++i )
        {
            histogram[i] = i*i;
        }
        r&=us_json_flatten_to_buffer( j, b );
        j->destroy(j);
    }
    {
        int i=0;
        int len = us_buffer_readable_count(b);
        for( i=0; i<len; ++i )
        {
            fprintf( stdout, "%c", us_buffer_peek(b,i) );
        }
        fprintf( stdout, "\n" );
    }
    return r;
}

int us_test_json_main ( int argc, char **argv )
{
    int r = 1;
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_json() )
            r = 0;
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}


/*@}*/




