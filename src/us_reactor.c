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


#include "us_reactor.h"

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


bool us_reactor_handler_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    self->m_allocator = allocator;
    self->m_finished=false;
    self->m_next = 0;
    self->m_reactor = 0;
    self->m_extra = extra;
    self->m_fd = fd;
    self->m_wake_on_readable = false;
    self->m_wake_on_writable = false;
    self->destroy = us_reactor_handler_destroy;
    self->tick = 0;
    self->readable = 0;
    self->writable = 0;
    return true;
}

us_reactor_handler_t * us_reactor_handler_create ( us_allocator_t *allocator )
{
    us_reactor_handler_t *item = 0;
    item = us_new( allocator, us_reactor_handler_t );
    if ( item != NULL )
    {
        if ( !us_reactor_handler_init ( item, allocator, -1, 0 ) )
        {
            us_delete ( allocator, item );
            item = 0;
        }
    }
    return item;
}

void us_reactor_handler_destroy ( us_reactor_handler_t *self )
{
    if ( self->m_fd!=-1 )
    {
        closesocket( self->m_fd );
    }
    us_delete( self->m_allocator, self );
}


bool us_reactor_init ( us_reactor_t *self, us_allocator_t *allocator, int max_handlers )
{
    self->m_allocator = allocator;
    self->destroy = us_reactor_destroy;
    self->m_handlers = 0;
    self->m_max_handlers = max_handlers;
#if defined(US_REACTOR_USE_POLL)
    self->m_poll_handlers = us_new_array( allocator, struct pollfd, max_handlers );
#elif defined(US_REACTOR_USE_SELECT)
    FD_ZERO( &self->m_read_fds );
    FD_ZERO( &self->m_write_fds );
#endif
    self->m_timeout = 0;
    self->add_item = us_reactor_add_item;
    self->remove_item = us_reactor_remove_item;
    self->poll = us_reactor_poll;
    self->m_num_handlers = 0;
#if defined(US_REACTOR_USE_POLL)
    if ( !self->m_poll_handlers )
    {
        return false;
    }
#endif
    return true;
}

void us_reactor_destroy ( us_reactor_t *self )
{
    while ( self->m_handlers )
    {
        self->remove_item ( self, self->m_handlers );
    }
#if defined(US_REACTOR_USE_POLL)
    us_delete( self->m_allocator, self->m_poll_handlers );
    self->m_poll_handlers = 0;
#endif
}


void us_reactor_collect_finished( us_reactor_t *self )
{
    us_reactor_handler_t **item;
    /* garbage collect handlers that are finished */
    item = &self->m_handlers;
    while ( *item )
    {
        if ( ( *item )->tick )
        {
            ( *item )->tick ( *item );
        }
        if ( ( *item )->m_finished == true || (*item)->m_fd==-1 )
        {
            us_reactor_handler_t *next = ( *item )->m_next;
            us_reactor_log_debug( "reactor item %p finished", (void *)*item );
            self->remove_item( self, *item );
            *item = next;
        }
        else
        {
            item = & ( *item )->m_next;
        }
    }
}

#if defined(US_REACTOR_USE_POLL )

void us_reactor_fill_poll( us_reactor_t *self )
{
    us_reactor_handler_t *item;
    int n = 0;
    item = self->m_handlers;
    while ( item )
    {
        if ( item->m_fd!=-1 && !item->m_finished )
        {
            struct pollfd *p;
            p = &self->m_poll_handlers[n++];
            p->events = 0;
            p->revents = 0;
            p->fd = item->m_fd;
            if ( item->m_wake_on_readable && item->readable != 0 )
            {
                us_reactor_log_debug( "item %p wor fd=%d", (void *)item, item->m_fd );
                p->events |= POLLIN;
            }
            if ( item->m_wake_on_writable && item->writable != 0 )
            {
                us_reactor_log_debug( "item %p wow fd=%d", (void *)item, item->m_fd );
                p->events |= POLLOUT;
            }
        }
        item = item->m_next;
    }
}

bool us_reactor_poll ( us_reactor_t *self, int timeout )
{
    bool r = false;
    us_reactor_collect_finished( self );
    us_reactor_fill_poll( self );
    if ( self->m_num_handlers > 0 )
    {
        int n;
#ifdef WIN32
        n = WSAPoll ( self->m_poll_handlers, self->m_num_handlers, timeout );
#else
        n = poll ( self->m_poll_handlers, self->m_num_handlers, timeout );
#endif
        if ( n < 0 && errno!=EINTR )
        {
            /* error doing poll, stop loop */
            us_reactor_log_debug( "error doing poll, errno=%d", errno );
            r = false;
        }
        if ( n > 0 )
        {
            /* some handlers to be handled */
            us_reactor_handler_t *item = self->m_handlers;
            int n;
            int num=self->m_num_handlers;
            for ( n=0; n<num; ++n, item=item->m_next )
            {
                struct pollfd *p = &self->m_poll_handlers[n];
                if ( p->fd != item->m_fd )
                {
                    us_reactor_log_error("item %p fd %d != p->fd", (void*)item, item->m_fd, p->fd );
                    abort();
                }
                if ( item->m_wake_on_readable && ( p->revents & POLLIN ) )
                {
                    us_reactor_log_debug( "item %p fd %d is readable", item, item->m_fd );
                    item->readable ( item );
                }
                if ( item->m_wake_on_writable && ( p->revents & POLLOUT ) )
                {
                    us_reactor_log_debug( "item %p fd %d is writable", item, item->m_fd );
                    item->writable ( item );
                }
                if ( (p->revents & POLLHUP) )
                {
                    us_reactor_log_debug("poll item %p fd %d got HUP: %d", (void *)item, item->m_fd, p->revents );
                    us_reactor_handler_finish( item );
                }
                if ( (p->revents & POLLERR) )
                {
                    us_reactor_log_debug("poll item %p fd %d got ERR: %d", (void *)item, item->m_fd, p->revents );
                    us_reactor_handler_finish( item );
                }
                if ( (p->revents & POLLNVAL) )
                {
                    us_reactor_log_error("poll item %p fd %d got NVAL: %d", (void *)item, item->m_fd, p->revents );
                    us_reactor_handler_finish( item );
                }
            }
            r = true;
        }
        if ( n == 0 )
        {
            /* there are handlers, nothing to do, timeout occurred */
            r = true;
        }
    }
    us_reactor_collect_finished( self );
    return r;
}
#elif defined(US_REACTOR_USE_SELECT)
bool us_reactor_poll ( us_reactor_t *self, int timeout )
{
    bool r = false;
    us_reactor_collect_finished( self );
    if ( self->m_num_handlers > 0 )
    {
        us_reactor_handler_t *item;
        int max_fd=0;
        int n=-1;
        struct timeval tv_timeout;
        item = self->m_handlers;
        tv_timeout.tv_usec = (timeout*1000)%1000;
        tv_timeout.tv_sec = (timeout/1000);
        FD_ZERO( &self->m_read_fds );
        FD_ZERO( &self->m_write_fds );
        while ( item )
        {
            if ( item->m_fd!=-1 && !item->m_finished )
            {
                if ( item->m_wake_on_readable && item->readable != 0 )
                {
                    FD_SET( item->m_fd, &self->m_read_fds );
                    if ( item->m_fd > max_fd )
                        max_fd = item->m_fd;
                }
                if ( item->m_wake_on_writable && item->writable != 0 )
                {
                    FD_SET( item->m_fd, &self->m_write_fds );
                    if ( item->m_fd > max_fd )
                        max_fd = item->m_fd;
                }
            }
            item = item->m_next;
        }
        do
        {
            n = select(max_fd+1, &self->m_read_fds, &self->m_write_fds, 0, &tv_timeout );
        }
        while (n<0 && (errno==EINTR || errno==EAGAIN ) );
        if ( n < 0 )
        {
            /* error doing select, stop loop */
            r = false;
        }
        if ( n > 0 )
        {
            /* some handlers to be handled */
            us_reactor_handler_t *item = self->m_handlers;
            while ( item )
            {
                if ( item->m_fd!=-1 && !item->m_finished)
                {
                    if ( item->m_wake_on_readable && FD_ISSET( item->m_fd, &self->m_read_fds ) )
                        item->readable ( item );
                }
                if ( item->m_fd!=-1 && !item->m_finished)
                {
                    if ( item->m_wake_on_writable && FD_ISSET( item->m_fd, &self->m_write_fds ) )
                        item->writable ( item );
                }
                item = item->m_next;
            }
            r = true;
        }
        if ( n == 0 )
        {
            /* there are handlers, nothing to do, timeout occurred */
            r = true;
        }
    }
    us_reactor_collect_finished( self );
    return r;
}
#endif


bool us_reactor_add_item (
    us_reactor_t *self,
    us_reactor_handler_t *item
)
{
    bool r = false;
    us_reactor_handler_t **node = &self->m_handlers;
    if ( self->m_num_handlers < self->m_max_handlers )
    {
        while ( *node )
        {
            node = & ( *node )->m_next;
        }
        self->m_num_handlers++;
        *node = item;
        item->m_reactor = self;
        r = true;
    }
    return r;
}

bool us_reactor_remove_item (
    us_reactor_t *self,
    us_reactor_handler_t *item
)
{
    bool r = false;
    us_reactor_handler_t **node = &self->m_handlers;
    us_reactor_log_debug( "removing reactor item %p fd %d finished %d", (void*)item, item->m_fd, item->m_finished );
    while ( *node )
    {
        if ( *node == item )
        {
            us_reactor_handler_t *next = ( *node )->m_next;
            self->m_num_handlers--;
            *node = next;
            r = true;
        }
        else
        {
            node = & ( *node )->m_next;
        }
    }
    item->destroy ( item );
    return r;
}

bool us_reactor_create_tcp_client (
    us_reactor_t *self,
    us_allocator_t *allocator,
    void *extra,
    int queue_buf_size,
    const char *server_host,
    const char *server_port,
    bool keep_open,
    us_reactor_handler_create_proc_t client_handler_create,
    us_reactor_handler_tcp_client_init_proc_t client_handler_init
)
{
    bool r=false;
    us_reactor_handler_tcp_client_t *handler = (us_reactor_handler_tcp_client_t *)client_handler_create(allocator);
    if ( handler )
    {
        r=client_handler_init( &handler->m_base.m_base, allocator, -1, extra, queue_buf_size, server_host, server_port, keep_open );
        if ( r )
        {
            r=self->add_item( self, &handler->m_base.m_base );
        }
    }
    return r;
}



bool us_reactor_create_server (
    us_reactor_t *self,
    us_allocator_t *allocator,
    const char *server_host,
    const char *server_port,
    int ai_socktype,
    void *client_extra,
    us_reactor_handler_create_proc_t server_handler_create,
    us_reactor_handler_init_proc_t server_handler_init
)
{
    int added_count = 0;
    bool r = true;
    int e;
    struct addrinfo *ai;
    struct addrinfo hints;
    memset ( &hints, '\0', sizeof ( hints ) );
#if defined(AI_ADDRCONFIG)
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
#else
    hints.ai_flags = AI_PASSIVE;
#endif
    hints.ai_socktype = ai_socktype;
    if ( server_host!=0 && *server_host=='\0' )
        server_host=0;
    if ( server_port!=0 && *server_port=='\0' )
        server_port=0;
    e = getaddrinfo ( server_host, server_port, &hints, &ai );
    if ( e == 0 )
    {
        struct addrinfo *cur_addr = ai;
        while ( cur_addr != NULL )
        {
            const int opt = 1;
            int fd;
            do
            {
                fd = socket ( cur_addr->ai_family, cur_addr->ai_socktype, cur_addr->ai_protocol );
            }
            while ( fd==-1 && errno==EINTR );
            if ( fd == -1 )
            {
                us_reactor_log_error( "socket: %s\n", strerror ( errno ) );
                freeaddrinfo ( ai );
                return false;
            }
            setsockopt ( fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof ( opt ) );
            if ( bind ( fd, cur_addr->ai_addr, cur_addr->ai_addrlen ) == 0 )
            {
                us_reactor_handler_t *item;
                item = server_handler_create(allocator);
                if ( !item )
                {
                    us_reactor_log_error( "item create failed" );
                    freeaddrinfo ( ai );
                    return false;
                }
                if ( ai_socktype==SOCK_STREAM )
                {
                    if ( listen ( fd, SOMAXCONN ) != 0 )
                    {
                        us_reactor_log_error( "listen: %s", strerror ( errno ) );
                        freeaddrinfo ( ai );
                        return false;
                    }
                }
                if ( server_handler_init ( item, allocator, fd, client_extra ) )
                {
                    if ( self->add_item ( self, item ) )
                    {
                        /* success, keep track of it */
                        char host_buf[1024];
                        char port_buf[256];
                        int e1;
                        e1=getnameinfo(
                               cur_addr->ai_addr, cur_addr->ai_addrlen,
                               host_buf, sizeof( host_buf),
                               port_buf, sizeof( port_buf),
                               NI_NUMERICHOST | NI_NUMERICSERV
                           );
                        if ( e1==0 )
                        {
                            us_reactor_log_debug( "Added listener on [%s]:%s", host_buf, port_buf );
                        }
                        else
                        {
                            us_reactor_log_debug( "unable to getnameinfo on succesful listening port: %s",
                                                  gai_strerror( e1 ) );
                        }
                        added_count++;
                    }
                    else
                    {
                        us_reactor_log_error( "unable to add item to reactor" );
                        freeaddrinfo ( ai );
                        return false;
                    }
                }
                else
                {
                    us_reactor_log_error( "unable to init server item" );
                    freeaddrinfo ( ai );
                    return false;
                }
            }
            else
            {
                if ( errno != EADDRINUSE )
                {
                    us_reactor_log_error( "bind: %s", strerror ( errno ) );
                    freeaddrinfo ( ai );
                    return false;
                }
                /* Note: if address is in use this is only an error if added_count is 0 at the end */
                closesocket ( fd );
            }
            cur_addr = cur_addr->ai_next;
        }
        freeaddrinfo ( ai );
    }
    else
    {
        us_reactor_log_error( "getaddrinfo: %s", gai_strerror ( e ) );
        return false;
    }
    if ( added_count > 0 )
    {
        r = true;
    }
    return r;
}


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
            e = connect( fd, cur_addr->ai_addr, cur_addr->ai_addrlen );
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

us_reactor_handler_t * us_reactor_handler_udp_create ( us_allocator_t *allocator  )
{
    return (us_reactor_handler_t *)us_new( allocator, us_reactor_handler_udp_t );
}

bool us_reactor_handler_udp_init ( us_reactor_handler_t *self_, us_allocator_t *allocator, int fd, void *extra, int32_t max_udp_packet_size )
{
    bool r=false;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    r = us_reactor_handler_init(self_, allocator, fd, extra);
    if ( r )
    {
        self_->destroy = us_reactor_handler_udp_destroy;
        self_->readable = us_reactor_handler_udp_readable;
        self->m_incoming_packet = us_buffer_create( allocator, max_udp_packet_size );
        if ( !self->m_incoming_packet )
        {
            r=false;
            self_->destroy( self_ );
        }
    }
    return r;
}

void us_reactor_handler_udp_destroy ( us_reactor_handler_t *self_ )
{
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    if ( self->m_incoming_packet )
        self->m_incoming_packet->destroy( self->m_incoming_packet );
    us_reactor_handler_destroy(self_);
}

bool us_reactor_handler_udp_readable( us_reactor_handler_t *self_ )
{
    bool r=false;
    int len;
    us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
    struct sockaddr_storage remote_addr;
    socklen_t remote_addrlen = sizeof( remote_addr );
    us_buffer_reset( self->m_incoming_packet );
    len = recvfrom(
              self->m_base.m_fd,
              (char *)self->m_incoming_packet->m_buffer,
              self->m_incoming_packet->m_max_length, 0,
              (struct sockaddr *)&remote_addr,
              &remote_addrlen
          );
    if ( len>0 )
    {
        self->m_incoming_packet->m_next_in = len;
        r=true;
        if ( self->packet_received )
        {
            self->packet_received( self, self->m_incoming_packet, (struct sockaddr *)&remote_addr, remote_addrlen );
        }
    }
    return r;
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

