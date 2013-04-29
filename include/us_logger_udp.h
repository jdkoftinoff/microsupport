#ifndef US_LOGGER_UDP_H
#define US_LOGGER_UDP_H

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

#include "us_world.h"
#include "us_logger.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     \addtogroup us_logger Logger
     */
    /*@{*/


# ifndef US_LOGGER_UDP_DEFAULT_DEST_ADDR
#  define US_LOGGER_UDP_DEFAULT_DEST_ADDR "255.255.255.255"
# endif

# ifndef US_LOGGER_UDP_DEFAULT_DEST_SERVICE
#  define US_LOGGER_UDP_DEFAULT_DEST_SERVICE "9001"
# endif


#if US_ENABLE_NETWORK && US_ENABLE_BSD_SOCKETS && US_ENABLE_PRINTING

    extern int us_logger_udp_socket;

    extern char us_logger_udp_buffer[1500];
    extern us_print_t *us_logger_udp_printer;
    extern us_printraw_t us_logger_udp_printer_impl;

    extern struct sockaddr_in us_logger_udp_dest_sockaddr;

    void us_log_udp_send();

    bool us_logger_udp_start ( const char *dest_addr, const char * service );
    void us_logger_udp_finish ( void );

    void us_log_error_udp ( const char *fmt, ... );
    void us_log_warn_udp ( const char *fmt, ... );
    void us_log_info_udp ( const char *fmt, ... );
    void us_log_debug_udp ( const char *fmt, ... );
    void us_log_trace_udp ( const char *fmt, ... );


# endif

    /*@}*/
#ifdef __cplusplus
}
#endif

#endif
