#include "us_world.h"
#include "us_packet.h"


/*
Copyright (c) 2012, Meyer Sound Laboratories, Inc.
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

us_packet_t * us_packet_create( us_allocator_t *allocator, size_t max_length )
{
    us_packet_t *r=0;
    us_packet_t *self = us_new( allocator, us_packet_t );
    if( self )
    {
        self->m_allocator = allocator;
        self->m_data = us_new_array( allocator, uint8_t, max_length );
        if( self->m_data )
        {
            self->m_max_length = max_length;
            us_packet_clear( self );
            self->destroy = us_packet_destroy;
            r=self;
        }
    }
    if( !r )
    {
        us_packet_destroy( self );
    }
    return r;
}

void us_packet_destroy( us_packet_t *self )
{
    if( self )
    {
        us_delete( self->m_allocator, self->m_data );
        us_delete( self->m_allocator, self );
    }
}

