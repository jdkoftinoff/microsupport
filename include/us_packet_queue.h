#ifndef US_PACKET_QUEUE_H
#define US_PACKET_QUEUE_H

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

#include "us_world.h"
#include "us_allocator.h"
#include "us_packet.h"

#ifdef __cplusplus
extern "C"
{
#endif


    /* \addtogroup packet_queue packet_queue */
    /*@{*/

    typedef struct us_packet_queue_s
    {
        us_allocator_t *m_allocator;
        size_t m_next_in;
        size_t m_next_out;
        size_t m_packet_count;
        size_t m_max_packets;
        us_packet_t **m_packets;
        void (*destroy)( struct us_packet_queue_s *self );
    } us_packet_queue_t;

    us_packet_queue_t * us_packet_queue_create(
        us_allocator_t *allocator,
        size_t num_packets,
        size_t max_packet_size
    );

    void us_packet_queue_destroy( us_packet_queue_t *self );

    void us_packet_queue_clear( us_packet_queue_t *self );

    static inline us_packet_t * us_packet_queue_get_next_in( us_packet_queue_t *self )
    {
        us_packet_t *r = 0;
        if( self->m_packet_count < self->m_max_packets )
        {
            r = self->m_packets[ self->m_next_in ];
        }
        return r;
    }

    static inline const us_packet_t * us_packet_queue_get_next_out( const us_packet_queue_t *self )
    {
        us_packet_t *r = 0;
        if( self->m_packet_count > 0 )
        {
            r = self->m_packets[ self->m_next_out ];
        }
        return r;
    }

    static inline size_t us_packet_queue_is_empty( const us_packet_queue_t *self )
    {
        return self->m_packet_count == 0 ? true : false;
    }

    static inline bool us_packet_queue_can_read( const us_packet_queue_t *self )
    {
        return self->m_packet_count > 0 ? true : false;
    }

    static inline bool us_packet_queue_can_write( const us_packet_queue_t *self )
    {
        return self->m_packet_count < (self->m_max_packets-1) ? true : false;
    }


    static inline void us_packet_queue_next_in( us_packet_queue_t *self )
    {
        self->m_next_in = (self->m_next_in + 1 ) % self->m_max_packets;
        ++self->m_packet_count;
    }

    static inline void us_packet_queue_next_out( us_packet_queue_t *self )
    {
        self->m_next_out = (self->m_next_out + 1 ) % self->m_max_packets;
        --self->m_packet_count;
    }

    /*@}*/

#ifdef __cplusplus
}
#endif

#endif
