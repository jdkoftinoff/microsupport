#ifndef US_OSC_IO_H
#define US_OSC_IO_H

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

#include "us_world.h"

#include "us_osc_msg.h"
#ifdef __cplusplus
extern "C" {
#endif
    


    /**
       \addtogroup us_osc_io_packet Helper classes for communicating OSC messages over packet based i/o
    */

    /*@{*/
    typedef struct us_osc_io_packet_s
    {
        void (*destroy)( struct us_osc_io_packet_s *self );
        us_allocator_t *m_allocator;
        us_buffer_t *m_incoming_buffer;
        us_buffer_t *m_outgoing_buffer;
    } us_osc_io_packet_t;

    us_osc_io_packet_t *us_osc_io_packet_create(
        us_allocator_t *allocator,
        int max_buf_size
    );

    bool us_osc_io_packet_init(
        us_osc_io_packet_t *self,
        us_allocator_t *allocator,
        int max_buf_size
    );

    void us_osc_io_packet_destroy(
        us_osc_io_packet_t *self
    );

    static inline bool us_osc_io_packet_is_msg(
        us_osc_io_packet_t *self
    )
    {
        return us_osc_msg_is_msg( self->m_incoming_buffer );
    }

    static inline bool us_osc_io_packet_is_msg_bundle(
        us_osc_io_packet_t *self
    )
    {
        return us_osc_msg_is_msg_bundle ( self->m_incoming_buffer );
    }


    bool us_osc_io_packet_get_msg(
        us_osc_io_packet_t *self,
        us_allocator_t *allocator,
        us_osc_msg_t **msg
    );

    bool us_osc_io_packet_get_msg_bundle(
        us_osc_io_packet_t *self,
        us_allocator_t *allocator,
        us_osc_msg_bundle_t **bundle
    );

    bool us_osc_io_packet_put_msg(
        us_osc_io_packet_t *self,
        us_osc_msg_t *msg
    );

    bool us_osc_io_packet_put_bundle(
        us_osc_io_packet_t *self,
        us_osc_msg_bundle_t *bundle
    );

    /*@}*/

    /**
       \addtogroup us_osc_io_slip_stream Helper classes for communicating OSC messages over slip framed byte streams
    */

    /*@{*/
    typedef struct us_osc_io_slip_stream_s
    {
        void (*destroy)( struct us_osc_io_slip_stream_s *self );
        us_allocator_t *m_allocator;
        us_buffer_t *m_incoming_buffer;
        us_buffer_t *m_outgoing_buffer;
    } us_osc_io_slip_stream_t;

    us_osc_io_slip_stream_t *us_osc_io_slip_stream_create(
        us_allocator_t *allocator,
        int max_buf_size
    );

    bool us_osc_io_slip_stream_init(
        us_osc_io_slip_stream_t *self,
        us_allocator_t *allocator,
        int max_buf_size
    );

    void us_osc_io_slip_stream_destroy(
        us_osc_io_packet_t *self
    );


    static inline bool us_osc_io_slip_stream_is_msg(
        us_osc_io_slip_stream_t *self
    )
    {
        return us_osc_msg_is_msg( self->m_incoming_buffer );
    }

    static inline bool us_osc_io_slip_stream_is_msg_bundle(
        us_osc_io_slip_stream_t *self
    )
    {
        return us_osc_msg_is_msg_bundle ( self->m_incoming_buffer );
    }


    bool us_osc_io_slip_stream_get_msg(
        us_osc_io_slip_stream_t *self,
        us_allocator_t *allocator,
        us_osc_msg_t **msg
    );

    bool us_osc_io_slip_stream_get_msg_bundle(
        us_osc_io_slip_stream_t *self,
        us_allocator_t *allocator,
        us_osc_msg_bundle_t **bundle
    );

    bool us_osc_io_slip_stream_put_msg(
        us_osc_io_slip_stream_t *self,
        us_osc_msg_t *msg
    );

    bool us_osc_io_slip_stream_put_bundle(
        us_osc_io_slip_stream_t *self,
        us_osc_msg_bundle_t *bundle
    );

    /*@}*/

    /**
       \addtogroup us_osc_io_packet_stream Helper classes for communicating OSC messages over slip framed byte streams
    */

    /*@{*/
    typedef struct us_osc_io_packet_stream_s
    {
        void (*destroy)( struct us_osc_io_packet_stream_s *self );
        us_allocator_t *m_allocator;
        us_buffer_t m_incoming_buffer;
        us_buffer_t m_outgoing_buffer;
    } us_osc_io_packet_stream_t;

    us_osc_io_packet_stream_t *us_osc_io_packet_stream_create(
        us_allocator_t *allocator,
        int max_buf_size
    );

    bool us_osc_io_packet_stream_init(
        us_osc_io_packet_stream_t *self,
        us_allocator_t *allocator,
        int max_buf_size
    );

    void us_osc_io_slip_stream_destroy(
        us_osc_io_packet_t *self
    );


    static inline bool us_osc_io_packet_stream_is_msg(
        us_osc_io_packet_stream_t *self
    )
    {
        return us_osc_msg_is_msg( &self->m_incoming_buffer );
    }

    static inline bool us_osc_io_packet_stream_is_msg_bundle(
        us_osc_io_packet_stream_t *self
    )
    {
        return us_osc_msg_is_msg_bundle ( &self->m_incoming_buffer );
    }


    bool us_osc_io_packet_stream_get_msg(
        us_osc_io_packet_stream_t *self,
        us_allocator_t *allocator,
        us_osc_msg_t **msg
    );

    bool us_osc_io_packet_stream_get_msg_bundle(
        us_osc_io_packet_stream_t *self,
        us_allocator_t *allocator,
        us_osc_msg_bundle_t **bundle
    );

    bool us_osc_io_packet_stream_put_msg(
        us_osc_io_packet_stream_t *self,
        us_osc_msg_t *msg
    );

    bool us_osc_io_packet_stream_put_bundle(
        us_osc_io_packet_stream_t *self,
        us_osc_msg_bundle_t *bundle
    );

    /*@}*/

#ifdef __cplusplus
}
#endif

#endif

