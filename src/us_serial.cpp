#include "us_world.hpp"
#include "us_net.hpp"
#include "us_serial.hpp"


#if defined(US_CONFIG_POSIX)
#include <termios.h>

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


bool us_serial_setup ( int fd, const char *serial_options )
{
    uint32_t requested_baud = 9600;
    uint32_t baud_code;
    const char *serial_fmt = 0;
    struct termios term;
    int e;
    do
    {
        e = tcgetattr ( fd, &term );
    }
    while ( e < 0 && errno == EINTR );
    if ( e < 0 )
    {
        us_log_error ( "ioctl TCGETS failed" );
        return false;
    }
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    term.c_iflag &= ~ ( BRKINT | ICRNL | INPCK | ISTRIP | IXON );
    term.c_iflag |= ( IGNBRK | IGNPAR );
    term.c_oflag &= ~OPOST;
    term.c_lflag &= ~ ( ECHO | ICANON | IEXTEN | ISIG );
    term.c_cflag &= ~ ( CSIZE );
    term.c_cflag |= ( CREAD | CLOCAL );
    /* extract serial option in form "BaudRate,FMT,FLOW"
     * where BaudRate is 1200,2400,4800,9600,19200,38400,57600,115200,230400, or 0
     * and FMT is one of {7,8}{N,I,O}{0,1,2}
     * and FLOW is one of H (hard) or S (soft) or N (none)
     */
    if ( serial_options && *serial_options )
    {
        requested_baud = ( uint32_t ) ( strtoul ( serial_options, 0, 10 ) );
        if ( requested_baud == 0 )
        {
            us_log_error ( "invalid baud rate" );
            return false;
        }
        serial_fmt = strchr ( serial_options, ',' );
        if ( serial_fmt )
        {
            serial_fmt++;
        }
        if ( strlen ( serial_fmt ) != 5 )
        {
            us_log_error ( "invalid serial format: for example use 8N1,H or 8N1,N " );
            return false;
        }
    }
    if ( serial_fmt == 0 )
    {
        serial_fmt = "8N1,N";
    }
    switch ( requested_baud )
    {
    case 31250:
        baud_code = 0; /* special case for MIDI */
        break;
    case 1200:
        baud_code = B1200;
        break;
    case 2400:
        baud_code = B2400;
        break;
    case 4800:
        baud_code = B4800;
        break;
    case 9600:
        baud_code = B9600;
        break;
    case 19200:
        baud_code = B19200;
        break;
    case 38400:
        baud_code = B38400;
        break;
#ifdef B57600
    case 57600:
        baud_code = B57600;
        break;
#endif
#ifdef B115200
    case 115200:
        baud_code = B115200;
        break;
#endif
#ifdef B230400
    case 230400:
        baud_code = B230400;
        break;
#endif
    default:
        us_log_error( "invalid baud rate: %d", requested_baud );
        return false;
        break;
    }
    if ( baud_code != 0 )
    {
        do
        {
            e = cfsetspeed ( &term, baud_code );
        }
        while ( e < 0 && errno == EINTR );
        if ( e < 0 )
        {
            us_log_error ( "error setting cfsetspeed" );
            return false;
        }
    }
    if ( serial_fmt[0] == '7' )
    {
        term.c_cflag |= CS7;
        term.c_cflag &= ~CS8;
    }
    else if ( serial_fmt[0] == '8' )
    {
        term.c_cflag &= ~CS7;
        term.c_cflag |= CS8;
    }
    else
    {
        us_log_error( "invalid bits: %c", serial_fmt[0] );
        return false;
    }
    if ( serial_fmt[1] == 'N' )
    {
        term.c_cflag &= ~PARENB;
    }
    else if ( serial_fmt[1] == 'E' )
    {
        term.c_cflag |= PARENB;
        term.c_cflag &= ~PARODD;
    }
    else if ( serial_fmt[1] == 'O' )
    {
        term.c_cflag |= PARENB;
        term.c_cflag |= PARODD;
    }
    else
    {
        us_log_error( "invalid parity: %c", serial_fmt[1] );
        return false;
    }
    if ( serial_fmt[2] == '1' )
    {
        term.c_cflag &= ~CSTOPB;
    }
    else if ( serial_fmt[2] == '2' )
    {
        term.c_cflag |= CSTOPB;
    }
    else
    {
        us_log_error( "invalid stop bits: %c", serial_fmt[2] );
        return false;
    }
    if ( serial_fmt[4] == 'H' )
    {
        term.c_cflag |= CRTSCTS;
    }
    else if ( serial_fmt[4] == 'N' )
    {
        term.c_cflag &= ~CRTSCTS;
    }
    else if ( serial_fmt[4] == 'S' )
    {
        term.c_iflag |= ( IXON | IXOFF | IXANY );
    }
    do
    {
        e = tcsetattr ( fd, TCSANOW, &term );
    }
    while ( e < 0 && errno == EINTR );
    if ( e < 0 )
    {
        us_log_error ( "ioctl TCSETS failed" );
        return false;
    }
    do
    {
        e = fcntl ( fd, F_SETFL, O_NONBLOCK );
    }
    while ( e < 0 && errno == EINTR );
    if ( e < 0 )
    {
        us_log_error( "fcntl F_SETFL O_NONBLOCK failed" );
        return false;
    }
    return true;
}

int us_serial_net_open ( const char *serial_device, const char *serial_options )
{
    int fd = -1;
    int e;
    const char *connect_addr = serial_device + 4;
    struct addrinfo *src_ai;
    struct addrinfo *dest_ai;
    char dest_ip[256] = "127.0.0.1";
    char dest_port[64] = "7777";
    ( void ) serial_options; /* ignored for now */
    if ( strlen ( connect_addr ) > 0 )
    {
        char *end_ip;
        strncpy ( dest_ip, connect_addr, sizeof ( dest_ip ) );
        dest_ip[ sizeof ( dest_ip )-1 ] = '\0';
        end_ip = strchr ( dest_ip, ',' );
        if ( end_ip )
        {
            *end_ip = '\0';
            strncpy ( dest_port, end_ip + 1, sizeof ( dest_port ) );
            dest_port[sizeof ( dest_port )-1 ] = '\0';
        }
    }
    src_ai = us_net_get_addrinfo ( "0.0.0.0", "0", SOCK_STREAM, false );
    if ( src_ai == 0 )
    {
        perror ( "src getaddrinfo:" );
        abort();
    }
    dest_ai = us_net_get_addrinfo (
                  dest_ip,
                  dest_port,
                  SOCK_STREAM,
                  false
              );
    if ( dest_ai == 0 )
    {
        perror ( "dest getaddrinfo:" );
        abort();
    }
    fd = us_net_create_tcp_socket ( src_ai, false );
    if ( fd < 0 )
    {
        perror ( "socket:" );
        abort();
    }
    do
    {
        e = connect ( fd, dest_ai->ai_addr, dest_ai->ai_addrlen );
    }
    while ( e < 0 && errno == EINTR );
    if ( e < 0 )
    {
        perror ( "connect to serial net" );
        close ( fd );
        abort();
    }
    do
    {
        e = fcntl ( fd, F_SETFL, O_NONBLOCK );
    }
    while ( e < 0 && errno == EINTR );
    if ( e < 0 )
    {
        perror ( "fcntl F_SETFL O_NONBLOCK failed" );
        close ( fd );
        abort();
    }
    freeaddrinfo ( src_ai );
    freeaddrinfo ( dest_ai );
    return fd;
}

int us_serial_open ( const char *serial_device, const char *serial_options )
{
    int fd = -1;
    if ( strncmp ( serial_device, "/dev/", 5 ) == 0 )
    {
        fd = open ( serial_device, O_RDWR );
        if ( fd >= 0 )
        {
            if ( !us_serial_setup ( fd, serial_options ) )
            {
                fd = -1;
            }
        }
    }
    else if ( strncmp ( serial_device, "tcp:", 4 ) == 0 )
    {
        fd = us_serial_net_open ( serial_device, serial_options );
    }
    return fd;
}

void us_serial_close ( int fd )
{
    if ( fd != -1 )
        close ( fd );
}

#endif
