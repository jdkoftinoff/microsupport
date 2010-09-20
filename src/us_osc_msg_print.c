#include "us_world.h"

#include "us_osc_msg.h"
#include "us_osc_msg_print.h"

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

#if US_ENABLE_PRINTING

bool
us_osc_msg_bundle_print(
    us_osc_msg_bundle_t *self,
    us_print_t *printer
)
{
    bool r=false;
    if ( self && printer)
    {
        us_osc_msg_t *cur = 0;
        r=true;
        r&=printer->printf( printer, "msg_bundle_t:  timetag: 0x%08lx%08lx\n", self->m_timetag_high, self->m_timetag_low );
        cur = self->m_first_msg;
        while (cur && r)
        {
            r &= cur->print( cur, printer );
            cur = cur->m_next;
        }
    }
    return r;
}


bool
us_osc_msg_print(
    us_osc_msg_t *self,
    us_print_t *printer
)
{
    bool r=false;
    if ( self && printer)
    {
        us_osc_msg_element_t *cur = 0;
        r=true;
        r&=printer->printf( printer, "msg_t: address: '%s'\n", self->m_address );
        cur = self->m_first_element;
        while (cur && r)
        {
            r &= cur->print( cur, printer );
            cur = cur->m_next;
        }
    }
    return r;
}

bool
us_osc_msg_element_print(
    us_osc_msg_element_t *self,
    us_print_t *printer
)
{
    return printer->printf( printer, "UNKNOWN TYPE: '%c'\n", self->m_code );
}

bool
us_osc_msg_element_a_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_a_t *self = (us_osc_msg_element_a_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: 0x%08lx%08lx\n",
               self->m_base.m_code,
               self->m_time_high,
               self->m_time_low
           );
}

bool
us_osc_msg_element_B_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    bool r=true;
    us_osc_msg_element_B_t *self = (us_osc_msg_element_B_t *)self_;
    r&=printer->printf(
           printer,
           "msg_element_B_t: "
       );
    r &= self->m_bundle->print( self->m_bundle, printer );
    r &=printer->printf( printer, "\n" );
    return r;
}


bool
us_osc_msg_element_b_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    bool r=true;
    int i;
    us_osc_msg_element_b_t *self = (us_osc_msg_element_b_t *)self_;
    r&=printer->printf(
           printer,
           "msg_element_%c_t: data length: %d, data: ",
           self->m_base.m_code,
           self->m_length
       );
    for ( i=0; i<self->m_length; ++i )
    {
        r&=printer->printf( printer, "%02x", self->m_data[i] );
    }
    r&=printer->printf( printer, "\n" );
    return r;
}

bool
us_osc_msg_element_d_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_d_t *self = (us_osc_msg_element_d_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: %g\n",
               self->m_base.m_code,
               self->m_value
           );
}

bool
us_osc_msg_element_f_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_f_t *self = (us_osc_msg_element_f_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: %f\n",
               self->m_base.m_code,
               self->m_value
           );
}

bool
us_osc_msg_element_h_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_h_t *self = (us_osc_msg_element_h_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: 0x%08lx%08lx\n",
               self->m_base.m_code,
               self->m_value_high,
               self->m_value_low
           );
}

bool
us_osc_msg_element_i_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_i_t *self = (us_osc_msg_element_i_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: %d\n",
               self->m_base.m_code,
               self->m_value
           );
}

bool
us_osc_msg_element_s_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    bool r=true;
    int i;
    us_osc_msg_element_s_t *self = (us_osc_msg_element_s_t *)self_;
    r&=printer->printf(
           printer,
           "msg_element_%c_t: string length: %d, value: '",
           self->m_base.m_code,
           self->m_length
       );
    for ( i=0; i<self->m_length; ++i )
    {
        r&=printer->printf(
               printer,
               "%c",
               self->m_value[i]
           );
    }
    r&=printer->printf( printer, "'\n" );
    return r;
}


bool
us_osc_msg_element_t_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_t_t *self = (us_osc_msg_element_t_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t: 0x%08lx%08lx\n",
               self->m_base.m_code,
               self->m_time_high,
               self->m_time_low
           );
}

bool
us_osc_msg_element_T_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_T_t *self = (us_osc_msg_element_T_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t:\n",
               self->m_base.m_code
           );
}

bool
us_osc_msg_element_F_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_F_t *self = (us_osc_msg_element_F_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t:\n",
               self->m_base.m_code
           );
}


bool
us_osc_msg_element_M_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    bool r=true;
    us_osc_msg_element_M_t *self = (us_osc_msg_element_M_t *)self_;
    r&=printer->printf(
           printer,
           "msg_element_M_t: "
       );
    r &= self->m_msg->print( self->m_msg, printer );
    r &=printer->printf( printer, "\n" );
    return r;
}


bool
us_osc_msg_element_N_print(
    us_osc_msg_element_t *self_,
    us_print_t *printer
)
{
    us_osc_msg_element_N_t *self = (us_osc_msg_element_N_t *)self_;
    return printer->printf(
               printer,
               "msg_element_%c_t:\n",
               self->m_base.m_code
           );
}

#endif

