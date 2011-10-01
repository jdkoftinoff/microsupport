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

#include "us_world.h"

#include "us_logger_syslog.h"
#include "us_cgi.h"


const char us_cgi_tohexdigit[16] =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

char const * const us_cgi_rfc3986_reserved_chars =
    "!*'();:@&=+$,/?%#[]";

bool us_cgi_is_escapable ( char c )
{
    const char *p;
    if ( ( c & 0x80 ) == 0x80 )
    {
        return true;
    }
    if ( isalnum ( c ) )
    {
        return false;
    }
    for ( p = us_cgi_rfc3986_reserved_chars; *p != 0; ++p )
    {
        if ( *p == c )
            return true;
    }
    return true;
}


bool us_cgi_unescape ( const char *src, int src_len, us_buffer_t *dest_buf )
{
    int i;
    int in_percent = 0;
    int partial = 0;
    for ( i = 0; i < src_len; ++i )
    {
        char c = src[i];
        if ( in_percent )
        {
            int v;
            c = toupper ( c );
            v = c - '0';
            if ( c > 9 )
                v = c - 'A' + 10;
            partial = partial << 4;
            partial += v;
            in_percent--;
            if ( in_percent == 0 )
            {
                c = ( char ) partial;
                if ( !us_buffer_append_byte(dest_buf, c ) )
                {
                    return false;
                }
            }
        }
        else if ( c == '%' )
        {
            in_percent = 2;
            partial = 0;
        }
        else
        {
            if ( !us_buffer_append_byte(dest_buf, c ) )
            {
                return false;
            }
        }
    }
    return true;
}

bool us_cgi_escape ( const char *src, int src_len, us_buffer_t *dest_buf )
{
    int i;
    for ( i = 0; i < src_len; ++i )
    {
        char c = src[i];
        if ( us_cgi_is_escapable ( c ) )
        {
            char escaped[4];
            escaped[0] = '%';
            escaped[1] = us_cgi_tohexdigit[ ( c>>4 ) &0xf ];
            escaped[2] = us_cgi_tohexdigit[ c&0xf ];
            if ( ! us_buffer_append_data(dest_buf,escaped,3) )
            {
                return false;
            }
        }
        else
        {
            if ( ! us_buffer_append_byte(dest_buf, c ) )
            {
                return false;
            }
        }
    }
    return true;
}


