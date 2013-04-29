#include "us_world.h"
#include "us_osc_sender_tcp.h"


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


bool us_osc_sender_tcp_init( us_osc_sender_tcp_t *self, us_reactor_handler_tcp_t *handler )
{
    self->m_base.can_send = us_osc_sender_tcp_can_send;
    self->m_base.send_msg = us_osc_sender_tcp_send_msg;
    self->m_base.form_and_send_msg = us_osc_sender_form_and_send_msg;
    self->m_handler = handler;
    self->m_max_size = 512;
    return true;
}

bool us_osc_sender_tcp_send_msg( us_osc_sender_t *self_,  const us_osc_msg_t *msg )
{
    us_osc_sender_tcp_t *self = (us_osc_sender_tcp_t *)self_;
    bool r;
    char data[1024];
    us_buffer_t buf;
    us_buffer_init( &buf, 0, data, sizeof(data) );
    r = msg->flatten( msg, &buf, 0 );
    if ( r )
    {
        int32_t len = (int32_t)us_buffer_readable_count( &buf );
        us_buffer_t *outgoing = &self->m_handler->m_outgoing_queue;
        us_buffer_append_int32( outgoing, len );
        us_buffer_write_buffer( outgoing, &buf );
        if ( !r )
        {
            us_log_error( "unable to write osc byte to outgoing queue" );
        }
    }
    else
    {
        us_log_error( "unable to flatten osc msg" );
    }
    return r;
}

bool us_osc_sender_tcp_can_send( us_osc_sender_t *self_ )
{
    us_osc_sender_tcp_t *self = (us_osc_sender_tcp_t *)self_;
    bool r;
    if ( us_buffer_writable_count( &self->m_handler->m_outgoing_queue )> self->m_max_size )
    {
        r=true;
    }
    else
    {
        r=false;
    }
    return r;
}

