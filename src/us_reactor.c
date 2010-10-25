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
#include "us_print.h"

/*#define US_REACTOR_TCP_TRACE*/

bool us_reactor_handler_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    self->m_allocator = allocator;
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
    /* nothing to do here */
}


bool us_reactor_init ( us_reactor_t *self, us_allocator_t *allocator, int max_handlers )
{
    self->m_allocator = allocator;
    self->destroy = us_reactor_destroy;
    self->m_handlers = 0;
    self->m_max_handlers = max_handlers;
#if defined(US_REACTOR_USE_POLL)
    self->m_poll_handlers = calloc ( sizeof ( struct pollfd ), max_handlers );
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
    free ( self->m_poll_handlers );
    self->m_poll_handlers = 0;
#endif
}


#if defined(US_REACTOR_USE_POLL )
bool us_reactor_poll ( us_reactor_t *self, int timeout )
{
    bool r = false;
    if ( self->m_num_handlers > 0 )
    {
        us_reactor_handler_t **item;
        /* garbage collect handlers that are closed */
        item = &self->m_handlers;
        while ( *item )
        {
            if ( ( *item )->tick )
            {
                ( *item )->tick ( *item );
            }
            if ( ( *item )->fd == -1 )
            {
                us_reactor_handler_t *next = ( *item )->m_next;
                ( *item )->destroy ( *item );
                free ( *item );
                self->m_num_handlers--;
                *item = next;
            }
            else
            {
                item = & ( *item )->m_next;
            }
        }
    }
    if ( self->m_num_handlers > 0 )
    {
        us_reactor_handler_t *item;
        int n = 0;
        item = self->m_handlers;
        while ( item )
        {
            struct pollfd *p;
            p = &self->m_poll_handlers[n++];
            p->events = 0;
            p->revents = 0;
            p->fd = item->m_fd;
            if ( item->m_wake_on_readable && item->readable != 0 )
                p->events |= POLLIN;
            if ( item->m_wake_on_writable && item->writable != 0 )
                p->events |= POLLOUT;
            item = item->m_next;
        }
#ifdef WIN32
        n = WSAPoll ( self->m_poll_handlers, self->m_num_handlers, timeout );
#else
        n = poll ( self->m_poll_handlers, self->m_num_handlers, timeout );
#endif
        if ( n < 0 )
        {
            /* error doing poll, stop loop */
            r = false;
        }
        if ( n > 0 )
        {
            /* some handlers to be handled */
            us_reactor_handler_t *item = self->m_handlers;
            int n = 0;
            while ( item )
            {
                struct pollfd *p = &self->m_poll_handlers[n++];
                if ( item->m_wake_on_readable && ( p->revents & POLLIN ) && item->readable != 0 )
                    item->readable ( item );
                if ( item->wake_on_writable && ( p->revents & POLLOUT ) && item->writable != 0 )
                    item->writable ( item );
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
    return r;
}
#elif defined(US_REACTOR_USE_SELECT)
bool us_reactor_poll ( us_reactor_t *self, int timeout )
{
    bool r = false;
    if ( self->m_num_handlers > 0 )
    {
        us_reactor_handler_t **item;
        /* garbage collect handlers that are closed */
        item = &self->m_handlers;
        while ( *item )
        {
            if ( ( *item )->tick )
            {
                ( *item )->tick ( *item );
            }
            if ( ( *item )->m_fd == -1 )
            {
                us_reactor_handler_t *next = ( *item )->m_next;
                ( *item )->destroy ( *item );
                free ( *item );
                self->m_num_handlers--;
                *item = next;
            }
            else
            {
                item = & ( *item )->m_next;
            }
        }
    }
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
            if( item->m_fd!=-1 )
            {
                if ( item->m_wake_on_readable && item->readable != 0 )
                {
                    FD_SET( item->m_fd, &self->m_read_fds );                
                    if( item->m_fd > max_fd )
                        max_fd = item->m_fd;
                }
                if ( item->m_wake_on_writable && item->writable != 0 )
                {
                    FD_SET( item->m_fd, &self->m_write_fds );
                    if( item->m_fd > max_fd )
                        max_fd = item->m_fd;
                }
            }
            item = item->m_next;
        }

        do 
        {            
            n = select(max_fd+1, &self->m_read_fds, &self->m_write_fds, 0, &tv_timeout );
        } while (n<0 && (errno==EINTR || errno==EAGAIN ) );

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
                if( item->m_fd!=-1 )
                {
                    if ( item->m_wake_on_readable && FD_ISSET( item->m_fd, &self->m_read_fds ) && item->readable )
                        item->readable ( item );
                }
                
                if( item->m_fd!=-1 )
                {                
                    if ( item->m_wake_on_writable && FD_ISSET( item->m_fd, &self->m_write_fds ) && item->writable )
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
    free ( item );
    return r;
}

bool us_reactor_create_tcp_client (
    us_reactor_t *self, 
    us_allocator_t *allocator,
    const char *server_host,
    const char *server_port,
    void *extra,
    us_reactor_handler_create_proc_t client_handler_create,
    us_reactor_handler_init_proc_t client_handler_init
)
{
    bool r=false;
    us_reactor_handler_t *handler = client_handler_create(allocator);

    if( handler )
    {
        int fd = us_reactor_tcp_blocking_connect(server_host, server_port);
        if( fd!=-1 )
        {
            r=client_handler_init( handler, allocator, fd, extra );

            if( r )
            {
                r=self->add_item( self, handler );
            }

            if( !r )
            {
                closesocket(fd);
                handler->destroy( handler );
            }
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
    e = getaddrinfo ( server_host, server_port, &hints, &ai );
    if ( e == 0 )
    {
        struct addrinfo *cur_addr = ai;
        while ( cur_addr != NULL )
        {
            const char opt = 1;
            int fd;
            
            do
            {
                fd = socket ( cur_addr->ai_family, cur_addr->ai_socktype, cur_addr->ai_protocol );
            } while ( fd==-1 && errno==EINTR );
            
            if ( fd == -1 )
            {
                us_stderr->printf( us_stderr, "socket: %s\n", strerror ( errno ) );
                freeaddrinfo ( ai );
                return false;
            }
            
            setsockopt ( fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof ( opt ) );
            
            if ( bind ( fd, cur_addr->ai_addr, cur_addr->ai_addrlen ) == 0 )
            {
                us_reactor_handler_t *item;
                item = server_handler_create(allocator);
                if ( !item )
                {
                    fprintf ( stderr, "item create failed\n" );
                    freeaddrinfo ( ai );
                    return false;
                }
                if ( listen ( fd, SOMAXCONN ) != 0 )
                {
                    fprintf ( stderr, "listen: %s\n", strerror ( errno ) );
                    freeaddrinfo ( ai );
                    return false;
                }
                if ( server_handler_init ( item, allocator, fd, client_extra ) )
                {
                    if ( self->add_item ( self, item ) )
                    {
                        /* success, keep track of it */
                        added_count++;
                    }
                    else
                    {
                        us_stderr->printf ( us_stderr, "unable to add item to reactor\n" );
                        freeaddrinfo ( ai );
                        return false;
                    }
                }
                else
                {
                    us_stderr->printf ( us_stderr, "unable to init server item\n" );
                    freeaddrinfo ( ai );
                    return false;
                }
            }
            else
            {
                if ( errno != EADDRINUSE )
                {
                    us_stderr->printf ( us_stderr, "bind: %s", strerror ( errno ) );
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
        us_stderr->printf ( us_stderr, "getaddrinfo: %s", strerror ( e ) );
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
    } while( accepted_fd==-1 && errno == EINTR );
    
    if ( accepted_fd != -1 )
    {
        us_reactor_handler_t *client_item = self->client_handler_create(self->m_base.m_allocator);
        if ( client_item != NULL )
        {
            r = self->client_handler_init ( client_item, self->m_base.m_allocator, accepted_fd, self->m_base.m_extra );
            if ( r )
            {
                us_reactor_handler_tcp_t * tcp_item = ( us_reactor_handler_tcp_t * ) client_item;
                r = self->m_base.m_reactor->add_item ( self->m_base.m_reactor, client_item );
                if ( r )
                {
                    if ( tcp_item->connected )
                    {
                        r = tcp_item->connected ( tcp_item, ( struct sockaddr * ) &rem, remlen );
                    }
                }
            }
            if ( !r )
            {
                fprintf ( stderr, "unabled to create tcp client handler\n" );
                closesocket ( client_item->m_fd );
                client_item->destroy ( client_item );
                free ( client_item );
            }
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
    int queue_buf_size,
    int xfer_buf_size
)
{
    bool r = false;
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    uint8_t *in_buf = NULL;
    uint8_t *out_buf = NULL;
    if ( us_reactor_handler_init ( self_, allocator, fd, extra ) )
    {
        self->m_base.destroy = us_reactor_handler_tcp_destroy;
        self->m_base.tick = us_reactor_handler_tcp_tick;
        self->m_base.readable = us_reactor_handler_tcp_readable;
        self->m_base.writable = us_reactor_handler_tcp_writable;
        self->m_base.m_wake_on_readable = true;
        self->m_xfer_buf_size = xfer_buf_size;
        self->close = us_reactor_handler_tcp_close;
        self->connected = 0;
        self->readable = 0;
        self->tick = 0;
        
        in_buf = us_new_array( allocator, uint8_t, queue_buf_size );
        out_buf = us_new_array( allocator, uint8_t, queue_buf_size );
        self->m_xfer_buf = us_new_array( allocator, char, xfer_buf_size );
    }
    if ( in_buf != NULL && out_buf != NULL && self->m_xfer_buf != NULL )
    {
        us_queue_init ( &self->m_incoming_queue, in_buf, queue_buf_size );
        us_queue_init ( &self->m_outgoing_queue, out_buf, queue_buf_size );
        r = true;
    }
    else
    {
        self->m_base.destroy ( &self->m_base );
        r = false;
    }
    return r;
}

void us_reactor_handler_tcp_destroy (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    self->close( self );
    us_delete( self->m_base.m_allocator, self->m_incoming_queue.m_buf );
    us_delete( self->m_base.m_allocator, self->m_outgoing_queue.m_buf );
    us_delete( self->m_base.m_allocator, self->m_xfer_buf );
    us_reactor_handler_destroy ( &self->m_base );
}

bool us_reactor_handler_tcp_tick (
    us_reactor_handler_t *self_
)
{
    bool r = true;
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    if ( us_queue_writable_count ( &self->m_incoming_queue ) >= self->m_xfer_buf_size )
    {
        self->m_base.m_wake_on_readable = true;
    }
    else
    {
        self->m_base.m_wake_on_readable = false;
    }
    if ( us_queue_can_read_byte ( &self->m_outgoing_queue ) )
    {
        self->m_base.m_wake_on_writable = true;
    }
    else
    {
        self->m_base.m_wake_on_writable = false;
    }
    if ( self->tick )
        r = self->tick ( self );
    return r;
}

bool us_reactor_handler_tcp_readable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    int len;
    
    do
    {
        len = recv ( self->m_base.m_fd, self->m_xfer_buf, self->m_xfer_buf_size, 0 );
    } while( len<0 && errno==EINTR );
    
    if ( len > 0 )
    {
#ifdef US_REACTOR_TCP_TRACE
        {
            int i;
            us_stderr->printf( us_stderr, "READ TCP DATA (len %d): ", len );
            for( i=0; i<len; i++ )
            {
                us_stderr->printf( us_stderr, "%02x ", self->xfer_buf[i] );
            }
            us_stderr->printf( us_stderr, "\n\n" );
        }
#endif
        us_queue_write ( &self->m_incoming_queue, ( uint8_t* ) self->m_xfer_buf, len );
        self->m_base.m_wake_on_readable = true;
        if ( self->readable )
        {
            self->readable ( self );
        }
    }
    else
    {    
        self->close( self );
    }
    return true;
}

void us_reactor_handler_tcp_close(
    us_reactor_handler_tcp_t *self_
    )
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    if( self->m_base.m_fd != -1 )
    {
        closesocket ( self->m_base.m_fd );
        self->m_base.m_fd = -1;
        if( self->closed )
        {
            self->closed( self );
        }
    }
}

bool us_reactor_handler_tcp_writable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r = false;
    int len;
    len = us_queue_readable_count ( &self->m_outgoing_queue );
    if ( len > 0 )
    {
        uint8_t *outgoing = us_queue_contig_read_ptr ( &self->m_outgoing_queue );
        int outgoing_len = us_queue_contig_readable_count ( &self->m_outgoing_queue );
#ifdef US_REACTOR_TCP_TRACE
        {
            int i;
            us_stderr->fprintf( us_stderr, "WRITE TCP DATA (len=%d): ", outgoing_len );
            for( i=0; i<len; i++ )
            {
                us_stderr->printf( us_stderr, "%02x ", outgoing[i] );
            }
            us_stderr->printf( us_stderr, "\n\n" );
        }
#endif

        do
        {
            len = send ( self->m_base.m_fd, (const char *)outgoing, outgoing_len, 0 );
        } while( len<0 && errno==EINTR );
        
        if ( len > 0 )
        {
#ifdef US_REACTOR_TCP_TRACE
            us_stderr->printf( us_stderr, "WROTE (len=%d): ", len );
#endif
            us_queue_skip ( &self->m_outgoing_queue, len );
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
                us_stderr->printf ( us_stderr, "socket: %s\n", strerror ( errno ) );
                break;
            }

            e = connect( fd, cur_addr->ai_addr, cur_addr->ai_addrlen );

            if( e==0 )
            {
                break;
            }

            cur_addr = cur_addr->ai_next;
        }
        freeaddrinfo ( ai );
    }
    else
    {
        fprintf ( stderr, "getaddrinfo: %s", strerror ( e ) );
    }
    return fd;
}


