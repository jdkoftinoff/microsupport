#include "us_world.h"
#include "us_buffer.h"
#include "us_print.h"
#include "us_logger_udp.h"


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
    * Neither the name of the Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if US_ENABLE_NETWORK && US_ENABLE_BSD_SOCKETS && US_ENABLE_PRINTING

int us_logger_udp_socket = -1;  /**! socket to use to send UDP packets */

char us_logger_udp_buffer[1500]; /**! buffer to hold text for one log line */
us_print_t *us_logger_udp_printer;  /**! pointer to printer object to use to form buffer text */
us_printraw_t us_logger_udp_printer_impl; /**! printer object to use to form buffer text */

struct sockaddr_in us_logger_udp_dest_sockaddr; /**! ip address/port list to send to */

void us_log_udp_send ( void );

void us_log_udp_send ( void )
{
    if ( us_logger_udp_socket != -1 )
    {
        if (
            sendto (
                us_logger_udp_socket,
                us_logger_udp_printer_impl.m_buffer,
                us_logger_udp_printer_impl.m_cur_length,
                0,
                ( void * ) &us_logger_udp_dest_sockaddr,
                sizeof ( us_logger_udp_dest_sockaddr )
            ) < 0
        )
        {
            /* Do nothing upon error here. What could we do anyways? log it? ;-) */
        }
    }
}

bool us_logger_udp_start ( const char *dest_addr, int16_t dest_port )
{
    bool r = false;
    us_logger_udp_printer = us_printraw_init (
                                &us_logger_udp_printer_impl,
                                us_logger_udp_buffer,
                                sizeof ( us_logger_udp_buffer )
                            );
    if ( us_logger_udp_printer )
    {
        /*
          First, find ip address to bind to
        */
        memset ( &us_logger_udp_dest_sockaddr, 0, sizeof ( us_logger_udp_dest_sockaddr ) );
        us_logger_udp_dest_sockaddr.sin_family = AF_INET;
        us_logger_udp_dest_sockaddr.sin_port = htons ( dest_port );
#ifdef _WIN32
        us_logger_udp_dest_sockaddr.sin_addr.S_un.S_addr = inet_addr ( dest_addr );
        if ( us_logger_udp_dest_sockaddr.sin_addr.S_un.S_addr != INADDR_NONE )
#else
        if ( inet_aton ( dest_addr, &us_logger_udp_dest_sockaddr.sin_addr ) )
#endif
        {
            /*
              Create socket for this address
             */
            us_logger_udp_socket = socket (
                AF_INET, SOCK_DGRAM, IPPROTO_UDP
            );

            if ( us_logger_udp_socket >= 0 )
            {
                r = true;
            }
        }
    }
    if ( r )
    {
        us_log_error_proc = us_log_error_udp;
        us_log_warn_proc = us_log_warn_udp;
        us_log_info_proc = us_log_info_udp;
        us_log_debug_proc = us_log_debug_udp;
        us_logger_finish = us_logger_udp_finish;
    }
    else
    {
        if ( us_logger_udp_socket != -1 )
        {
            closesocket ( us_logger_udp_socket );
        }
        us_logger_udp_socket = -1;
    }
    return r;
}

void us_logger_udp_finish()
{
    if ( us_logger_udp_socket != -1 )
    {
        closesocket ( us_logger_udp_socket );
        us_logger_udp_socket = -1;
    }
    us_log_error_proc = us_log_null;
    us_log_warn_proc = us_log_null;
    us_log_info_proc = us_log_null;
    us_log_debug_proc = us_log_null;
    us_logger_finish = us_logger_null_finish;
}

void us_log_error_udp ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf ( us_logger_udp_printer, "ERROR:\t" );
    us_logger_udp_printer->vprintf ( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end ( ap );
}

void us_log_warn_udp ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf ( us_logger_udp_printer, "WARNING:\t" );
    us_logger_udp_printer->vprintf ( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end ( ap );
}

void us_log_info_udp ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf ( us_logger_udp_printer, "INFO:\t" );
    us_logger_udp_printer->vprintf ( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end ( ap );
}

void us_log_debug_udp ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    us_logger_udp_printer_impl.m_cur_length = 0;
    us_logger_udp_printer->printf ( us_logger_udp_printer, "DEBUG:\t" );
    us_logger_udp_printer->vprintf ( us_logger_udp_printer, fmt, ap );
    us_log_udp_send();
    va_end ( ap );
}

#endif
