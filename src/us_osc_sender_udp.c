#include "us_world.h"
#include "us_osc_sender_udp.h"
#include "us_net.h"
#include "us_crc32.h"


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


bool us_osc_sender_udp_init(
    us_osc_sender_udp_t *self,
    struct addrinfo *bind_addr,
    struct addrinfo *dest_addr
)
{
    self->m_base.destroy = us_osc_sender_udp_destroy;
    self->m_base.can_send = us_osc_sender_udp_can_send;
    self->m_base.send_msg = us_osc_sender_udp_send_msg;
    self->m_base.form_and_send_msg = us_osc_sender_form_and_send_msg;
    self->m_fd = us_net_create_udp_socket( bind_addr, false );
    self->m_bind_addr = bind_addr;
    self->m_dest_addr = dest_addr;
    return self->m_fd>=0;
}

void us_osc_sender_udp_destroy( us_osc_sender_t *self_)
{
    us_osc_sender_udp_t *self = (us_osc_sender_udp_t *)self_;
    closesocket( self->m_fd );
}

#define CRC_SIZE 4
bool us_osc_sender_udp_send_msg( us_osc_sender_t *self_,  const us_osc_msg_t *msg )
{
    us_osc_sender_udp_t *self = (us_osc_sender_udp_t *)self_;
    bool r;
    char data[1024];
    us_buffer_t buf;
    us_buffer_init( &buf, 0, data+CRC_SIZE , sizeof(data)-CRC_SIZE );
    r = msg->flatten( msg, &buf, 0 );
    if( r )
    {
        ssize_t len = us_buffer_readable_count( &buf );
        ssize_t sent;

        *((uint32_t *)&data[0])=us_crc32(0,&data[CRC_SIZE],len);
        len+=CRC_SIZE;
        sent = sendto( self->m_fd, data, len, 0, self->m_dest_addr->ai_addr, self->m_dest_addr->ai_addrlen );
        if( sent!=len )
        {
            r=false;
        }
        if( !r )
        {
            us_log_error( "unable to write osc message to UDP socket" );
        }
    }
    else
    {
        us_log_error( "unable to flatten osc msg" );
    }
    return r;
}

bool us_osc_sender_udp_can_send( us_osc_sender_t *self_ )
{
    us_osc_sender_udp_t *self = (us_osc_sender_udp_t *)self_;
    return self->m_fd>=0 && self->m_dest_addr!=0;
}

