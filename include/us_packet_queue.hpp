#ifndef US_PACKET_QUEUE_HPP
#define US_PACKET_QUEUE_HPP

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

#include "us_world.hpp"
#include "us_packet.hpp"
#include "us_packet_queue.h"

namespace microsupport
{
class packet_queue_t
{
public:
    packet_queue_t(
        us_allocator_t *allocator,
        size_t num_packets,
        size_t max_packet_size=1500
        )
    {
        queue=us_packet_queue_create( allocator, num_packets, max_packet_size );
        if( !queue )
        {
            throw std::bad_alloc();
        }
    }

    ~packet_queue_t()
    {
        if( queue )
        {
            queue->destroy( queue );
        }
    }

    void clear()
    {
        us_packet_queue_clear( queue );
    }

    packet_t * get_next_in()
    {
        return reinterpret_cast<packet_t *>(us_packet_queue_get_next_in( queue ));
    }

    const us_packet_t *get_next_out() const
    {
        return reinterpret_cast<const packet_t *>(us_packet_queue_get_next_out( queue ));
    }

    bool is_empty() const
    {
        return us_packet_queue_is_empty( queue );
    }

    bool can_read() const
    {
        return us_packet_queue_can_read( queue );
    }

    bool can_write() const
    {
        return us_packet_queue_can_write( queue );
    }

    void next_in()
    {
        us_packet_queue_next_in( queue );
    }

    void next_out()
    {
        us_packet_queue_next_out( queue );
    }

private:
    us_packet_queue_t *queue;
};
}

#endif
