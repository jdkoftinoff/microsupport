#include "us_world.h"

#include "us_slip.h"

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

us_slip_decoder_t *
us_slip_decoder_create (
    us_allocator_t *allocator,
    int32_t max_packet_len,
    us_slip_decoder_proc packet_formed_callback
)
{
    us_slip_decoder_t *self;
    us_buffer_t *buffer;
    self = us_new ( allocator, us_slip_decoder_t );
    if ( self )
    {
        buffer = us_buffer_create ( allocator, max_packet_len );
        if ( buffer )
        {
            self = us_slip_decoder_init ( self, buffer, packet_formed_callback );
        }
        else
        {
            self = 0;
        }
    }
    return self;
}

us_slip_decoder_t *
us_slip_decoder_init (
    us_slip_decoder_t *self,
    us_buffer_t *buffer,
    us_slip_decoder_proc packet_formed_callback
)
{
    self->m_state = us_slip_state_before_packet;
    self->m_buffer = buffer;
    self->m_packet_formed_callback = packet_formed_callback;
    return self;
}

void us_slip_decoder_reset (
    us_slip_decoder_t *self
)
{
    self->m_state = us_slip_state_before_packet;
    us_buffer_reset ( self->m_buffer );
}

int us_slip_decoder_parse (
    us_slip_decoder_t *self,
    const uint8_t *data,
    int data_len
)
{
    int packet_count = 0;
    int i;
    for ( i = 0; i < data_len; ++i )
    {
        uint8_t c = data[i];
        switch ( self->m_state )
        {
        case us_slip_state_before_packet:
            /* ignore data until we see a valid end of packet */
            if ( c == US_SLIP_END )
            {
                /* found end of packet byte, so start scanning for real data */
                self->m_state = us_slip_state_in_packet;
            }
            break;
        case us_slip_state_in_packet:
            /* put unescaped data into buffer and check for END */
            if ( c == US_SLIP_END && self->m_buffer->m_cur_length != 0 )
            {
                /* found a complete non empty packet - dispatch it */
                self->m_packet_formed_callback ( self, self->m_buffer );
                /* clear the buffer */
                us_buffer_reset ( self->m_buffer );
                /* increment packet counter */
                packet_count++;
                /* and stay in this state */
            }
            else if ( c == US_SLIP_ESC )
            {
                /* found escape code, so go into ESC state */
                self->m_state = us_slip_state_in_esc;
            }
            else
            {
                /* any other character gets appended into the buffer */
                us_buffer_append_byte ( self->m_buffer, c );
            }
            break;
        case us_slip_state_in_esc:
            if ( c == US_SLIP_ESC_END )
            {
                /* code was an escaped END character, so append an END char */
                us_buffer_append_byte ( self->m_buffer, US_SLIP_END );
                self->m_state = us_slip_state_in_packet;
            }
            else if ( c == US_SLIP_ESC_ESC )
            {
                /* code was an escaped ESC character, so append an ESC char */
                us_buffer_append_byte ( self->m_buffer, US_SLIP_ESC );
                self->m_state = us_slip_state_in_packet;
            }
            else
            {
                /* unknown escape character, this is a protocol error so reset the buffer and state machine */
                us_slip_decoder_reset ( self );
            }
            break;
        default:
            /* unknown state, reset everything */
            us_slip_decoder_reset ( self );
            break;
        }
    }
    return packet_count;
}

bool us_slip_encode (
    us_buffer_t *dest_buffer,
    us_buffer_t *src_buffer
)
{
    bool r = true;
    int32_t i;
    if ( src_buffer->m_cur_length > 0 )
    {
        /* start the packet with an END code to ensure packet beginning is seen */
        r &= us_buffer_append_byte ( dest_buffer, US_SLIP_END );
        for ( i = 0; i < src_buffer->m_cur_length; ++i )
        {
            uint8_t c;
            /* if appending buffer failed stop trying */
            if ( !r )
                break;
            /* get character to encode */
            c = src_buffer->m_buffer[i];
            if ( c == US_SLIP_END )
            {
                /* Send ESC code with code for escaped END */
                r &= us_buffer_append_byte ( dest_buffer, US_SLIP_ESC );
                r &= us_buffer_append_byte ( dest_buffer, US_SLIP_ESC_END );
            }
            else if ( c == US_SLIP_ESC )
            {
                /* Send ESC code with code for escaped ESC */
                r &= us_buffer_append_byte ( dest_buffer, US_SLIP_ESC );
                r &= us_buffer_append_byte ( dest_buffer, US_SLIP_ESC_ESC );
            }
            else
            {
                /* Send raw data */
                r &= us_buffer_append_byte ( dest_buffer, c );
            }
        }
        /* end the packet with an END code */
        r &= us_buffer_append_byte ( dest_buffer, US_SLIP_END );
    }
    return r;
}


