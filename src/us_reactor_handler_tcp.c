#include "us_world.h"

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


#include "us_reactor_handler_tcp.h"

#include "us_logger.h"

/* #define US_REACTOR_DEBUG */

#if defined(US_REACTOR_DEBUG)
#define us_reactor_log_debug us_log_debug
#define us_reactor_log_trace us_log_trace
#define us_reactor_log_tracepoint() us_log_tracepoint()
#define US_REACTOR_TCP_TRACE_TX
#define US_REACTOR_TCP_TRACE_RX
#else
#define us_reactor_log_debug(...) do { } while(0)
#define us_reactor_log_trace(...) do { } while(0)
#define us_reactor_log_tracepoint(...) do { } while(0)
#endif

#define us_reactor_log_error us_log_error




us_reactor_handler_t *us_reactor_handler_tcp_server_create ( us_allocator_t *allocator )
{
    return (us_reactor_handler_t *)us_new( allocator, us_reactor_handler_tcp_server_t );
}

bool us_reactor_handler_tcp_server_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *client_extra,
    us_reactor_handler_create_proc_t client_handler_create,
    us_reactor_handler_init_proc_t client_handler_init
)
{
    us_reactor_handler_tcp_server_t *self = ( us_reactor_handler_tcp_server_t * ) self_;
    if ( us_reactor_handler_init ( &self->m_base, allocator, fd, client_extra ) )
    {
        self->client_handler_init = client_handler_init;
        self->client_handler_create = client_handler_create;
        self->m_base.readable = us_reactor_handler_tcp_server_readable;
        self->m_base.writable = 0;
        self->m_base.m_wake_on_readable = true;
        return true;
    }
    else
    {
        return false;
    }
}

bool us_reactor_handler_tcp_server_readable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_server_t *self = ( us_reactor_handler_tcp_server_t * ) self_;
    bool r = false;
    struct sockaddr_storage rem;
    socklen_t remlen = sizeof ( rem );
    int accepted_fd;
    do
    {
        accepted_fd = accept ( self->m_base.m_fd, ( struct sockaddr * ) &rem, &remlen );
    }
    while ( accepted_fd==-1 && errno == EINTR );
    if ( accepted_fd != -1 )
    {
        us_reactor_handler_t *client_item = self->client_handler_create(self->m_base.m_allocator);
        us_reactor_handler_tcp_t * tcp_item = ( us_reactor_handler_tcp_t * ) client_item;
        if ( client_item != NULL )
        {
            r = self->client_handler_init ( client_item, self->m_base.m_allocator, accepted_fd, self->m_base.m_extra );
        }
        if ( r && tcp_item->connected )
        {
            r = tcp_item->connected ( tcp_item, ( struct sockaddr * ) &rem, remlen );
        }
        if ( r )
        {
            r = self->m_base.m_reactor->add_item ( self->m_base.m_reactor, client_item );
        }
        if ( !r && client_item )
        {
            client_item->destroy( client_item );
        }
        if ( !client_item )
        {
            /* no client item, therefore fd must close */
            us_reactor_log_error( "accepted incoming socket %d but no client created, closing", accepted_fd );
            closesocket( accepted_fd );
        }
    }
    return r;
}


us_reactor_handler_t * us_reactor_handler_tcp_create ( us_allocator_t *allocator )
{
    return (us_reactor_handler_t *)us_new( allocator, us_reactor_handler_tcp_t );
}

bool us_reactor_handler_tcp_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra,
    int queue_buf_size
)
{
    bool r = false;
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    uint8_t *in_buf = NULL;
    uint8_t *out_buf = NULL;
    r=us_reactor_handler_init ( self_, allocator, fd, extra );
    if ( r )
    {
        self->m_base.destroy = us_reactor_handler_tcp_destroy;
        self->m_base.tick = us_reactor_handler_tcp_tick;
        self->m_base.readable = us_reactor_handler_tcp_readable;
        self->m_base.writable = us_reactor_handler_tcp_writable;
        self->m_base.m_wake_on_readable = true;
        self->close = us_reactor_handler_tcp_close;
        self->connected = 0;
        self->readable = 0;
        self->writable = 0;
        self->tick = 0;
        self->incoming_eof = 0;
        self->closed = 0;
        in_buf = us_new_array( allocator, uint8_t, queue_buf_size );
        out_buf = us_new_array( allocator, uint8_t, queue_buf_size );
    }
    else
    {
        us_reactor_log_error( "initializing tcp reactor handler failed" );
    }
    if ( r )
    {
        if ( in_buf != NULL && out_buf != NULL )
        {
            us_buffer_init ( &self->m_incoming_queue, 0, in_buf, queue_buf_size );
            us_buffer_init ( &self->m_outgoing_queue, 0, out_buf, queue_buf_size );
            r = true;
        }
        else
        {
            us_reactor_log_error( "allocation of tcp buffers failed (%d)", queue_buf_size );
            us_delete( allocator, in_buf );
            us_delete( allocator, out_buf );
            r=false;
        }
    }
    return r;
}

void us_reactor_handler_tcp_destroy (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    self->close( self );
    us_delete( self->m_base.m_allocator, self->m_incoming_queue.m_buffer );
    us_delete( self->m_base.m_allocator, self->m_outgoing_queue.m_buffer );
    us_reactor_handler_destroy ( &self->m_base );
}

bool us_reactor_handler_tcp_tick (
    us_reactor_handler_t *self_
)
{
    bool r = true;
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    int32_t incoming_count = us_buffer_contig_writable_count ( &self->m_incoming_queue );
    bool outgoing_available = us_buffer_can_read_byte ( &self->m_outgoing_queue );
    us_reactor_log_tracepoint();
    /* If we have space in our incoming queue, wake up when the socket is readable */
    if ( incoming_count >0 )
    {
        self->m_base.m_wake_on_readable = true;
    }
    else
    {
        self->m_base.m_wake_on_readable = false;
    }
    /* if we have data to send, wake up if the socket is writable */
    /* and if we have a tcp writable function active and we do not have data to send, wake when the socket is writable */
    if ( outgoing_available || (!outgoing_available && self->writable) )
    {
        self->m_base.m_wake_on_writable = true;
    }
    else
    {
        self->m_base.m_wake_on_writable = false;
    }
    if ( self->tick )
        r = self->tick ( self );
    us_reactor_log_trace( "fd %d WOR: %d WOW: %d", self->m_base.m_fd, self->m_base.m_wake_on_readable, self->m_base.m_wake_on_writable );
    return r;
}

bool us_reactor_handler_tcp_readable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    int len;
    int max_len = us_buffer_contig_writable_count( &self->m_incoming_queue );
    void *p = us_buffer_contig_write_ptr( &self->m_incoming_queue );
    us_reactor_log_tracepoint();
    if ( max_len==0 )
    {
        us_log_error( "logic error: contig_writable is 0 but socket is readable" );
        return false;
    }
    do
    {
        len = recv ( self->m_base.m_fd, (char *)p, max_len, 0 );
    }
    while ( len<0 && errno==EINTR );
    if ( len > 0 )
    {
        us_buffer_contig_written( &self->m_incoming_queue,len );
        us_reactor_log_debug( "socket %d read %d bytes, next_in=%d, next_out=%d",
                              self->m_base.m_fd,
                              len,
                              self->m_incoming_queue.m_next_in,
                              self->m_incoming_queue.m_next_out
                            );
        self->m_base.m_wake_on_readable = true;
        if ( self->readable )
        {
            self->readable ( self );
        }
    }
    else
    {
        if ( self->incoming_eof )
        {
            if ( !self->incoming_eof( self ) )
            {
                self->close(self);
            }
        }
        else
        {
            self->close( self );
        }
    }
    return true;
}

void us_reactor_handler_tcp_close(
    us_reactor_handler_tcp_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    us_reactor_log_tracepoint();
    if ( !self->m_base.m_finished )
    {
        if ( self->closed )
        {
            self->closed( self );
        }
        self->m_base.m_finished = true;
    }
}

bool us_reactor_handler_tcp_writable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r = false;
    int len;
    us_reactor_log_tracepoint();
    len = us_buffer_readable_count ( &self->m_outgoing_queue );
    if ( len==0 && self->writable )
    {
        self->writable( self );
        len = us_buffer_readable_count ( &self->m_outgoing_queue );
    }
    if ( len > 0 )
    {
        const uint8_t *outgoing = us_buffer_contig_read_ptr ( &self->m_outgoing_queue );
        int outgoing_len = us_buffer_contig_readable_count ( &self->m_outgoing_queue );
        do
        {
            len = send ( self->m_base.m_fd, (const char *)outgoing, outgoing_len, 0 );
        }
        while ( len<0 && errno==EINTR );
        if ( len > 0 )
        {
#ifdef US_REACTOR_TCP_TRACE_TX
            us_reactor_log_debug( "WROTE (len=%d): ", len );
#endif
            us_buffer_skip ( &self->m_outgoing_queue, len );
            r = true;
        }
    }
    if ( r == false )
    {
        self->close( self );
    }
    return r;
}


int us_reactor_connect_or_open( const char *fname )
{
    int fd=0;
    if ( strncmp(fname,":tcp:", 5 )==0 )
    {
        char hostname[256]="localhost";
        char port[32]="0";
        char *p;
        strcpy( hostname, fname+5 );
        p=strchr(hostname,':');
        if ( p )
        {
            *p='\0';
            strcpy( port, p+1 );
        }
        fd=us_reactor_tcp_blocking_connect(hostname, port);
    }
    else
    {
        do
        {
#ifdef WIN32
            fd=_open(fname,O_RDWR);
#else
            fd=open(fname, O_RDWR |O_NONBLOCK );
#endif
        }
        while (fd==-1 && errno==EINTR );
    }
    return fd;
}


int us_reactor_tcp_blocking_connect (
    const char *server_host,
    const char *server_port
)
{
    int fd=-1;
    int e;
    struct addrinfo *ai;
    struct addrinfo hints;
    memset ( &hints, '\0', sizeof ( hints ) );
#if defined(AI_ADDRCONFIG)
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
#else
    hints.ai_flags = AI_PASSIVE;
#endif
    hints.ai_socktype = SOCK_STREAM;
    e = getaddrinfo ( server_host, server_port, &hints, &ai );
    if ( e == 0 )
    {
        struct addrinfo *cur_addr = ai;
        while ( cur_addr != NULL )
        {
            fd = socket ( cur_addr->ai_family, cur_addr->ai_socktype, cur_addr->ai_protocol );
            if ( fd == -1 )
            {
                us_reactor_log_error( "socket: %s\n", strerror ( errno ) );
                break;
            }
            do
            {
                e = connect( fd, cur_addr->ai_addr, cur_addr->ai_addrlen );
            }
            while (e==-1 && errno == EINTR );
            if ( e==0 )
            {
                break;
            }
            closesocket(fd);
            fd=-1;
            cur_addr = cur_addr->ai_next;
        }
        freeaddrinfo ( ai );
    }
    else
    {
        us_reactor_log_error( "getaddrinfo: %s", gai_strerror ( e ) );
    }
    return fd;
}

us_reactor_handler_t * us_reactor_handler_tcp_client_create ( us_allocator_t *allocator )
{
    return (us_reactor_handler_t *)us_new( allocator, us_reactor_handler_tcp_client_t );
}


bool us_reactor_handler_tcp_client_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra,
    int queue_buf_size,
    const char *client_host,
    const char *client_port,
    bool keep_open
)
{
    us_reactor_handler_tcp_client_t *self = (us_reactor_handler_tcp_client_t *)self_;
    bool r=true;
    r&=us_reactor_handler_tcp_init(
           self_,
           allocator,
           fd,
           extra,
           queue_buf_size
       );
    if ( r )
    {
        self->m_client_host = us_strdup( allocator, client_host );
        self->m_client_port = us_strdup( allocator, client_port );
        self->m_is_connected = false;
        if ( keep_open )
        {
            self->m_keep_open = true;
            self->m_try_once=false;
        }
        else
        {
            self->m_keep_open = false;
            self->m_try_once=true;
        }
        self->m_base.m_base.destroy = us_reactor_handler_tcp_client_destroy;
        self->m_base.tick = us_reactor_handler_tcp_client_tick;
        self->m_base.closed = us_reactor_handler_tcp_client_closed;
    }
    return r;
}

void us_reactor_handler_tcp_client_destroy (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_client_t *self = (us_reactor_handler_tcp_client_t *)self_;
    us_delete( self->m_base.m_base.m_allocator, self->m_client_host );
    us_delete( self->m_base.m_base.m_allocator, self->m_client_port );
    us_reactor_handler_tcp_destroy( self_ );
}

bool us_reactor_handler_tcp_client_tick (
    us_reactor_handler_tcp_t *self_
)
{
    us_reactor_handler_tcp_client_t *self = (us_reactor_handler_tcp_client_t *)self_;
    if ( !self->m_is_connected && ( self->m_keep_open || self->m_try_once) )
    {
        /* TODO: Ideally make this non-blocking connect */
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        self->m_try_once = false;
        us_reactor_log_debug( "tcp client connecting '[%s]:%s'", self->m_client_host, self->m_client_port );
        self_->m_base.m_fd = us_reactor_tcp_blocking_connect( self->m_client_host, self->m_client_port );
        if ( self_->m_base.m_fd >=0 )
        {
            us_reactor_log_debug( "tcp client connected  '[%s]:%s'", self->m_client_host, self->m_client_port );
            self->m_is_connected=true;
        }
        else
        {
            us_reactor_log_debug( "tcp client connection '[%s]:%s' failed: %s", self->m_client_host, self->m_client_port, strerror(errno) );
        }
    }
    return true;
}


void us_reactor_handler_tcp_client_closed (
    struct us_reactor_handler_tcp_s *self_
)
{
    us_reactor_handler_tcp_client_t *self = (us_reactor_handler_tcp_client_t *)self_;
    self->m_is_connected=false;
    if ( self->m_keep_open )
    {
        us_reactor_log_debug( "tcp client connection '[%s]:%s' was closed, will re-connect", self->m_client_host, self->m_client_port );
        self->m_base.m_base.m_finished=false;
    }
    else
    {
        us_reactor_log_debug( "tcp client connection '[%s]:%s' was closed, will NOT re-connect", self->m_client_host, self->m_client_port );
    }
}

