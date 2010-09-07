#include "us_world.h"

#include "us_parse.h"

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


bool us_parse_hexdig ( uint8_t *val, const char *buf, int buf_len, int *buf_pos )
{
    bool r = false;

    if ( *buf_pos < buf_len )
    {
        char c = buf[*buf_pos];
        ++ ( *buf_pos );

        if ( c >= '0' && c <= '9' )
        {
            *val = c - '0';
            r = true;
        }
        else
        {
            c = c & 0xdf;

            if ( c >= 'A' && c <= 'F' )
            {
                *val = c - 'A' + 10;
                r = true;
            }
        }
    }

    return r;
}

bool us_parse_hexoctet ( uint8_t *val, const char *buf, int buf_len, int *buf_pos )
{
    bool r = true;
    uint8_t high = 0;
    uint8_t low = 0;
    r &= us_parse_hexdig ( &high, buf, buf_len, buf_pos );
    r &= us_parse_hexdig ( &low, buf, buf_len, buf_pos );

    if ( r )
    {
        *val = ( high << 4 ) + low;
    }

    return r;
}


