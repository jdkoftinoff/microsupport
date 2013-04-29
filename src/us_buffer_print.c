#include "us_world.h"

#include "us_buffer_print.h"
#include "us_buffer.h"
#include "us_print.h"
#include "us_logger_syslog.h"

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

#if US_ENABLE_PRINTING

bool
us_buffer_print (
    us_buffer_t *self,
    us_print_t *printer
)
{
    bool r = false;
    if ( self && printer )
    {
        size_t len=us_buffer_readable_count(self);
        r = true;
        r &= printer->printf (
                 printer,
                 "length: 0x%x\nContents:\n",
                 len
             );
        if ( r )
        {
            size_t i;
            for ( i = 0; i < len; ++i )
            {
                r &= printer->printf ( printer, "%02x ", us_buffer_peek( self, i ) );
                if ( !r )
                    break;
            }
            r &= printer->printf ( printer, "\n" );
        }
    }
    return r;
}



bool
us_buffer_print_string (
    us_buffer_t *self,
    us_print_t *printer
)
{
    bool r = false;
    if ( self && printer )
    {
        size_t len=us_buffer_readable_count(self);
        r = true;
        r &= printer->printf (
                 printer,
                 "length: 0x%x\nContents:\n",
                 len
             );
        if ( r )
        {
            size_t i;
            for ( i = 0; i < len; ++i )
            {
                r &= printer->printf ( printer, "%c", us_buffer_peek(self,i) );
                if ( !r )
                    break;
            }
            r &= printer->printf ( printer, "\n" );
        }
    }
    return r;
}

#endif

