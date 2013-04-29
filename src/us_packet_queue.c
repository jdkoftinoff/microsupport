#include "us_world.h"
#include "us_packet_queue.h"

/*
Copyright (c) 2012, J.D. Koftinoff Software, Ltd.
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


us_packet_queue_t * us_packet_queue_create(
    us_allocator_t *allocator,
    size_t num_packets,
    size_t max_packet_size
)
{
    us_packet_queue_t *r=0;
    us_packet_queue_t *self = us_new( allocator, us_packet_queue_t );
    if( self )
    {
        size_t i;

        self->m_allocator = allocator;
        self->m_next_in = 0;
        self->m_next_out = 0;
        self->m_packet_count = 0;
        self->m_max_packets = num_packets;
        self->destroy = us_packet_queue_destroy;

        self->m_packets = us_new_array( allocator, us_packet_t *, num_packets );
        if( self->m_packets )
        {
            bool good=true;
            for( i=0; i<num_packets; ++i )
            {
                self->m_packets[i] = 0;
            }
            for ( i=0; i<num_packets; ++i )
            {
                self->m_packets[i] = us_packet_create( allocator, max_packet_size );
                if( !self->m_packets[i] )
                {
                    good=false;
                    break;
                }
            }
            if( good )
            {
                r=self;
            }
        }
    }
    if( !r )
    {
        us_packet_queue_destroy( self );
    }
    return r;
}

void us_packet_queue_destroy( us_packet_queue_t *self )
{
    if( self )
    {
        size_t i;
        if( self->m_packets )
        {
            for ( i=0; i < self->m_max_packets; ++i )
            {
                if( self->m_packets[i] )
                {
                    self->m_packets[i]->destroy( self->m_packets[i] );
                    self->m_packets[i] = 0;
                }
            }
            us_delete( self->m_allocator, self->m_packets );
            self->m_packets = 0;
        }
        us_delete( self->m_allocator, self );
    }
}

void us_packet_queue_clear( us_packet_queue_t *self )
{
    self->m_next_in=0;
    self->m_next_out=0;
    self->m_packet_count=0;
}

