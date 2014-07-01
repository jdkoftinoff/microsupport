#include "us_world.h"
#include "us_queue.h"

#include "us_logger_printer.h"

#include "us_testutil.h"
#include "us_test_packet_queue_main.h"

/*
 Copyright (c) 2012, J.D. Koftinoff Software, Ltd.
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

/** \addtogroup us_test_packet_queue */
/*@{*/

static bool us_test_packet_queue( us_allocator_t * );

#define FAIL_IF( A )                                                                                                           \
    if ( A )                                                                                                                   \
    {                                                                                                                          \
        us_log_error( "FAIL FILE %s LINE %d: %s", __FILE__, __LINE__, #A );                                                    \
        goto error;                                                                                                            \
    }

static bool us_test_packet_queue( us_allocator_t *a )
{
    bool r = false;
    us_packet_queue_t *q = us_packet_queue_create( a, 8, 512 );
    if ( q )
    {
        us_packet_t *p;
        const us_packet_t *p1;
        int k;

        for ( k = 0; k < 10; ++k )
        {
            int i;
            FAIL_IF( !us_packet_queue_is_empty( q ) );
            FAIL_IF( !us_packet_queue_can_write( q ) );
            FAIL_IF( us_packet_queue_can_read( q ) );
            for ( i = 0; i < 6; ++i )
            {
                us_log_info( "w%d in:%d out:%d", i, q->m_next_in, q->m_next_out );
                p = us_packet_queue_get_next_in( q );
                FAIL_IF( p == 0 );
                p->m_data[0] = i;
                us_packet_queue_next_in( q );
                FAIL_IF( us_packet_queue_is_empty( q ) );
                FAIL_IF( !us_packet_queue_can_write( q ) );
                FAIL_IF( !us_packet_queue_can_read( q ) );
            }
            us_log_info( "w%d in:%d out:%d", 6, q->m_next_in, q->m_next_out );
            p = us_packet_queue_get_next_in( q );
            FAIL_IF( p == 0 );
            p->m_data[0] = 6;
            us_packet_queue_next_in( q );
            FAIL_IF( us_packet_queue_is_empty( q ) );
            FAIL_IF( us_packet_queue_can_write( q ) );
            FAIL_IF( !us_packet_queue_can_read( q ) );

            for ( i = 0; i < 6; ++i )
            {
                us_log_info( "r%d in:%d out:%d", i, q->m_next_in, q->m_next_out );
                p1 = us_packet_queue_get_next_out( q );
                FAIL_IF( p1 == 0 );
                FAIL_IF( p1->m_data[0] != i );
                us_packet_queue_next_out( q );
                FAIL_IF( us_packet_queue_is_empty( q ) );
                FAIL_IF( !us_packet_queue_can_write( q ) );
                FAIL_IF( !us_packet_queue_can_read( q ) );
            }
            us_log_info( "r%d in:%d out:%d", 6, q->m_next_in, q->m_next_out );
            p1 = us_packet_queue_get_next_out( q );
            FAIL_IF( p1 == 0 );
            FAIL_IF( p1->m_data[0] != 6 );
            us_packet_queue_next_out( q );
            FAIL_IF( !us_packet_queue_is_empty( q ) );
            FAIL_IF( !us_packet_queue_can_write( q ) );
            FAIL_IF( us_packet_queue_can_read( q ) );
        }

        r = true;
    }
error:
    return r;
}

int us_test_packet_queue_main( int argc, const char **argv )
{
    int r = 1;
    if ( us_testutil_start( 20480, 20480, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level( US_LOG_LEVEL_DEBUG );
        us_log_info( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        if ( us_test_packet_queue( us_testutil_sys_allocator ) )
            r = 0;
        us_log_info( "Finishing us_test_packet_queue" );
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}

/*@}*/
