#include "us_world.h"

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

#include "us_reactor_handler_udp.h"

#include "us_logger.h"

us_reactor_handler_t *us_reactor_handler_udp_create( us_allocator_t *allocator )
{
    return (us_reactor_handler_t *)us_new( allocator, us_reactor_handler_udp_t );
}

/**
*/
bool us_reactor_handler_udp_init_ex( us_reactor_handler_t *self_,
                                     us_allocator_t *allocator,
                                     void *extra,
                                     const char *ethernet_port,
                                     struct addrinfo *localaddr,
                                     struct addrinfo *multicastaddr,
                                     size_t input_packets,
                                     size_t output_packets,
                                     size_t max_packet_size )
{
    bool r = false;
    int fd;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;

    self->m_incoming_packets = 0;
    self->m_outgoing_packets = 0;
    self->tick = 0;
    self->queue_readable = us_reactor_handler_udp_queue_readable;
    self->packet_received = 0;
    fd = us_net_create_multicast_tx_udp_socket( localaddr, multicastaddr, ethernet_port );
    if ( fd >= 0 )
    {
        r = us_reactor_handler_init( self_, allocator, fd, extra );
        self_->tick = us_reactor_handler_udp_tick;
        self_->destroy = us_reactor_handler_udp_destroy;
        self_->readable = us_reactor_handler_udp_readable;
        self_->writable = us_reactor_handler_udp_writable;
        if ( r )
        {
            self->m_incoming_packets = us_packet_queue_create( allocator, input_packets, max_packet_size );
            self->m_outgoing_packets = us_packet_queue_create( allocator, output_packets, max_packet_size );
            self_->m_wake_on_readable = true;
            self_->m_wake_on_writable = false;
        }
    }
    if ( !r )
    {
        us_reactor_handler_udp_destroy( self_ );
    }
    return r;
}

bool us_reactor_handler_udp_init(
    us_reactor_handler_t *self_, us_allocator_t *allocator, int fd, void *extra, size_t max_packet_size )
{
    bool r = false;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;

    self->m_incoming_packets = 0;
    self->m_outgoing_packets = 0;
    self->tick = 0;
    self->queue_readable = 0;
    self->packet_received = 0;
    if ( fd >= 0 )
    {
        r = us_reactor_handler_init( self_, allocator, fd, extra );
        self_->tick = us_reactor_handler_udp_tick;
        self_->destroy = us_reactor_handler_udp_destroy;
        self_->readable = us_reactor_handler_udp_readable;
        self_->writable = us_reactor_handler_udp_writable;
        if ( r )
        {
            self->m_incoming_packets = us_packet_queue_create( allocator, 1, max_packet_size );
            self->m_outgoing_packets = us_packet_queue_create( allocator, 1, max_packet_size );
            self_->m_wake_on_readable = true;
            self_->m_wake_on_writable = false;
        }
    }
    if ( !r )
    {
        us_reactor_handler_udp_destroy( self_ );
    }
    return r;
}

void us_reactor_handler_udp_destroy( us_reactor_handler_t *self_ )
{
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;

    if ( self )
    {
        if ( self->m_incoming_packets )
        {
            self->m_incoming_packets->destroy( self->m_incoming_packets );
        }
        if ( self->m_outgoing_packets )
        {
            self->m_outgoing_packets->destroy( self->m_outgoing_packets );
        }
        us_reactor_handler_destroy( &self->m_base );
    }
}

bool us_reactor_handler_udp_readable( us_reactor_handler_t *self_ )
{
    bool r = false;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    if ( self )
    {
        us_packet_queue_t *q = self->m_incoming_packets;
        if ( us_packet_queue_can_write( q ) )
        {
            us_packet_t *p = us_packet_queue_get_next_in( q );
            if ( p )
            {
                ssize_t s;
                socklen_t remote_addrlen = sizeof( struct sockaddr_storage );

                us_packet_clear( p );
                p->m_src_address.m_type = us_packet_address_tcp;
                p->m_dest_address.m_type = us_packet_address_none;

                do
                {
                    s = recvfrom( self->m_base.m_fd,
                                  p->m_data,
                                  p->m_max_length,
                                  0,
                                  (struct sockaddr *)&p->m_src_address.tcp.m_addr,
                                  &remote_addrlen );
                } while ( s < 0 && errno == EINTR );

                if ( s >= 0 )
                {
                    p->m_src_address.tcp.m_if_id = -1;
                    p->m_src_address.tcp.m_len = remote_addrlen;
                    us_packet_queue_next_in( q );
                    r = true;
                }
                else
                {
                    us_log_error( "unable to receive packet" );
                }
            }
            else
            {
                us_log_error( "unable to get packet space in queue, lost packet" );
            }
        }
        else
        {
            us_log_error( "incoming packet queue full, lost packet" );
        }
    }
    return r;
}

bool us_reactor_handler_udp_writable( us_reactor_handler_t *self_ )
{
    bool r = false;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    if ( self )
    {
        us_packet_queue_t *q = self->m_outgoing_packets;
        if ( us_packet_queue_can_read( q ) )
        {
            ssize_t len;
            const us_packet_t *p = us_packet_queue_get_next_out( q );
            do
            {
                len = sendto( self->m_base.m_fd,
                              p->m_data,
                              p->m_length,
                              0,
                              (struct sockaddr *)&p->m_dest_address.tcp.m_addr,
                              p->m_dest_address.tcp.m_len );
            } while ( len < 0 && errno == EINTR );

            if ( len >= 0 )
            {
                us_packet_queue_next_out( q );
                r = true;
            }
            else
            {
                us_log_error( "Unable to send packet on writable socket" );
            }
        }
    }
    return r;
}

bool us_reactor_handler_udp_tick( us_reactor_handler_t *self_ )
{
    bool r = true;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    if ( self )
    {
        if ( self->tick )
        {
            r = self->tick( self );
        }
        if ( r )
        {
            if ( self->queue_readable )
            {
                /* only accept incoming packets if we have space in our incoming queue
                  and we have space in our writable queue for a potential response
                  */
                if ( us_packet_queue_can_write( self->m_incoming_packets )
                     && us_packet_queue_can_write( ( self->m_outgoing_packets ) ) )
                {
                    r = self->queue_readable( self );
                }
            }

            /* if we have packets in our outgoing queue, wake on writable */
            if ( us_packet_queue_can_read( self->m_outgoing_packets ) )
            {
                self_->m_wake_on_writable = true;
            }
            else
            {
                self_->m_wake_on_writable = false;
            }
        }
    }
    return r;
}

bool us_reactor_handler_udp_queue_readable( us_reactor_handler_udp_t *self )
{
    bool r = false;
    if ( self && self->packet_received )
    {
        us_packet_queue_t *q = self->m_incoming_packets;
        while ( us_packet_queue_can_read( q ) )
        {
            const us_packet_t *p = us_packet_queue_get_next_out( q );
            if ( p )
            {
                self->packet_received( self, p, self->m_outgoing_packets );
            }
            if ( !r )
            {
                break;
            }
        }
    }
    return r;
}
