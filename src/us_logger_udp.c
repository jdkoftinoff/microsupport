#include "us_world.h"
#include "us_buffer.h"
#include "us_print.h"
#include "us_logger_udp.h"

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
    * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if US_ENABLE_NETWORK &&US_ENABLE_BSD_SOCKETS &&US_ENABLE_PRINTING

int us_logger_udp_socket = -1; /**! socket to use to send UDP packets */

char us_logger_udp_buffer[1500];          /**! buffer to hold text for one log line */
us_print_t *us_logger_udp_printer;        /**! pointer to printer object to use to form buffer text */
us_printraw_t us_logger_udp_printer_impl; /**! printer object to use to form buffer text */

struct addrinfo *us_logger_udp_dest_addrinfo;

void us_log_udp_send( void );

void us_log_udp_send( void )
{
    if ( us_logger_udp_socket != -1 )
    {
        if ( sendto( us_logger_udp_socket,
                     us_logger_udp_printer_impl.m_buffer,
                     us_logger_udp_printer_impl.m_cur_length,
                     0,
                     us_logger_udp_dest_addrinfo->ai_addr,
                     us_logger_udp_dest_addrinfo->ai_addrlen ) < 0 )
        {
            /* Do nothing upon error here. What could we do anyways? log it? ;-) */
        }
    }
}

bool us_logger_udp_start( const char *dest_addr, const char *service )
{
    bool r = false;

    struct addrinfo hints, *ai;
    int gai;

    us_logger_udp_printer
        = us_printraw_init( &us_logger_udp_printer_impl, us_logger_udp_buffer, sizeof( us_logger_udp_buffer ) );

    if ( us_logger_udp_printer )
    {
        /* resolve address */
        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        gai = getaddrinfo( dest_addr, service, &hints, &ai );
        if ( gai )
        {
            fprintf( stderr,
                     "getaddrinfo(): %s service:%s: %s\n",
                     dest_addr != NULL ? dest_addr : "(null)",
                     service,
                     gai_strerror( gai ) );
            return ( r );
        }
        us_logger_udp_socket = -1;
        if ( us_logger_udp_dest_addrinfo != NULL )
        {
            freeaddrinfo( us_logger_udp_dest_addrinfo );
            us_logger_udp_dest_addrinfo = NULL;
        }
        us_logger_udp_dest_addrinfo = ai;
        us_logger_udp_socket = socket( us_logger_udp_dest_addrinfo->ai_family,
                                       us_logger_udp_dest_addrinfo->ai_socktype,
                                       us_logger_udp_dest_addrinfo->ai_protocol );
        if ( us_logger_udp_socket >= 0 )
        {
            r = true;
        }
    }
    if ( r )
    {
        us_log_error_proc = us_log_error_udp;
        us_log_warn_proc = us_log_warn_udp;
        us_log_info_proc = us_log_info_udp;
        us_log_debug_proc = us_log_debug_udp;
        us_log_trace_proc = us_log_trace_udp;
        us_logger_finish = us_logger_udp_finish;
    }
    else
    {
        if ( us_logger_udp_socket != -1 )
        {
            closesocket( us_logger_udp_socket );
        }
        us_logger_udp_socket = -1;
    }
    return r;
}

void us_logger_udp_finish()
{
    if ( us_logger_udp_socket != -1 )
    {
        closesocket( us_logger_udp_socket );
        us_logger_udp_socket = -1;
    }

    if ( us_logger_udp_dest_addrinfo != NULL )
    {
        freeaddrinfo( us_logger_udp_dest_addrinfo );
        us_logger_udp_dest_addrinfo = NULL;
    }

    us_log_error_proc = us_log_null;
    us_log_warn_proc = us_log_null;
    us_log_info_proc = us_log_null;
    us_log_debug_proc = us_log_null;
    us_logger_finish = us_logger_null_finish;
}

void us_log_error_udp( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf( us_logger_udp_printer, "ERROR:\t" );
    us_logger_udp_printer->vprintf( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end( ap );
}

void us_log_warn_udp( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf( us_logger_udp_printer, "WARNING:\t" );
    us_logger_udp_printer->vprintf( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end( ap );
}

void us_log_info_udp( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf( us_logger_udp_printer, "INFO:\t" );
    us_logger_udp_printer->vprintf( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end( ap );
}

void us_log_debug_udp( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf( us_logger_udp_printer, "DEBUG:\t" );
    us_logger_udp_printer->vprintf( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end( ap );
}

void us_log_trace_udp( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf( us_logger_udp_printer, "TRACE:\t" );
    us_logger_udp_printer->vprintf( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end( ap );
}

#endif
