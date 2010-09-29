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

bool us_reactor_handler_init (
    us_reactor_handler_t *self,
    int fd,
    void *extra
)
{
    self->next = 0;
    self->reactor = 0;
    self->destroy = us_reactor_handler_destroy;
    self->extra = extra;
    self->fd = fd;
    self->wake_on_readable = false;
    self->wake_on_writable = false;
    self->tick = 0;
    self->readable = 0;
    self->writable = 0;
    return true;
}

us_reactor_handler_t * us_reactor_handler_create ( void )
{
    us_reactor_handler_t *item = 0;
    item = calloc ( sizeof ( us_reactor_handler_t ), 1 );
    if ( item != NULL )
    {
        if ( !us_reactor_handler_init ( item, -1, 0 ) )
        {
            free ( item );
            item = 0;
        }
    }
    return item;
}

void us_reactor_handler_destroy ( us_reactor_handler_t *self )
{
    /* nothing to do here */
}


bool us_reactor_init ( us_reactor_t *self, int max_handlers )
{
    self->destroy = us_reactor_destroy;
    self->handlers = 0;
    self->max_handlers = max_handlers;
    self->poll_handlers = calloc ( sizeof ( struct pollfd ), max_handlers );
    self->timeout = 0;
    self->add_item = us_reactor_add_item;
    self->remove_item = us_reactor_remove_item;
    self->poll = us_reactor_poll;
    self->num_handlers = 0;
    if ( !self->poll_handlers )
    {
        return false;
    }
    return true;
}

void us_reactor_destroy ( us_reactor_t *self )
{
    while ( self->handlers )
    {
        self->remove_item ( self, self->handlers );
    }
    free ( self->poll_handlers );
    self->poll_handlers = 0;
}

bool us_reactor_poll ( us_reactor_t *self, int timeout )
{
    bool r = false;
    if ( self->num_handlers > 0 )
    {
        us_reactor_handler_t **item;
        /* garbage collect handlers that are closed */
        item = &self->handlers;
        while ( *item )
        {
            if ( ( *item )->tick )
            {
                ( *item )->tick ( *item );
            }
            if ( ( *item )->fd == -1 )
            {
                us_reactor_handler_t *next = ( *item )->next;
                ( *item )->destroy ( *item );
                free ( *item );
                self->num_handlers--;
                *item = next;
            }
            else
            {
                item = & ( *item )->next;
            }
        }
    }
    if ( self->num_handlers > 0 )
    {
        us_reactor_handler_t *item;
        int n = 0;
        item = self->handlers;
        while ( item )
        {
            struct pollfd *p;
            p = &self->poll_handlers[n++];
            p->events = 0;
            p->revents = 0;
            p->fd = item->fd;
            if ( item->wake_on_readable && item->readable != 0 )
                p->events |= POLLIN;
            if ( item->wake_on_writable && item->writable != 0 )
                p->events |= POLLOUT;
            item = item->next;
        }
#ifdef WIN32
        n = WSAPoll ( self->poll_handlers, self->num_handlers, timeout );
#else
        n = poll ( self->poll_handlers, self->num_handlers, timeout );
#endif
        if ( n < 0 )
        {
            /* error doing poll, stop loop */
            r = false;
        }
        if ( n > 0 )
        {
            /* some handlers to be handled */
            us_reactor_handler_t *item = self->handlers;
            int n = 0;
            while ( item )
            {
                struct pollfd *p = &self->poll_handlers[n++];
                if ( item->wake_on_readable && ( p->revents & POLLIN ) && item->readable != 0 )
                    item->readable ( item );
                if ( item->wake_on_writable && ( p->revents & POLLOUT ) && item->writable != 0 )
                    item->writable ( item );
                item = item->next;
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

bool us_reactor_add_item (
    us_reactor_t *self,
    us_reactor_handler_t *item
)
{
    bool r = false;
    us_reactor_handler_t **node = &self->handlers;
    if ( self->num_handlers < self->max_handlers )
    {
        while ( *node )
        {
            node = & ( *node )->next;
        }
        self->num_handlers++;
        *node = item;
        item->reactor = self;
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
    us_reactor_handler_t **node = &self->handlers;
    while ( *node )
    {
        if ( *node == item )
        {
            us_reactor_handler_t *next = ( *node )->next;
            self->num_handlers--;
            *node = next;
            r = true;
        }
        else
        {
            node = & ( *node )->next;
        }
    }
    item->destroy ( item );
    free ( item );
    return r;
}


bool us_reactor_create_server (
    us_reactor_t *self,
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
            int fd = socket ( cur_addr->ai_family, cur_addr->ai_socktype, cur_addr->ai_protocol );
            if ( fd == -1 )
            {
                fprintf ( stderr, "socket: %s\n", strerror ( errno ) );
                return false;
            }
            setsockopt ( fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof ( opt ) );
            if ( bind ( fd, cur_addr->ai_addr, cur_addr->ai_addrlen ) == 0 )
            {
                us_reactor_handler_t *item;
                item = server_handler_create();
                if ( !item )
                {
                    fprintf ( stderr, "item create failed\n" );
                    return false;
                }
                if ( listen ( fd, SOMAXCONN ) != 0 )
                {
                    fprintf ( stderr, "listen: %s\n", strerror ( errno ) );
                    return false;
                }
                if ( server_handler_init ( item, fd, client_extra ) )
                {
                    if ( self->add_item ( self, item ) )
                    {
                        /* success, keep track of it */
                        added_count++;
                    }
                    else
                    {
                        fprintf ( stderr, "unable to add item to reactor\n" );
                        return false;
                    }
                }
                else
                {
                    fprintf ( stderr, "unable to init server item\n" );
                    return false;
                }
            }
            else
            {
                if ( errno != EADDRINUSE )
                {
                    fprintf ( stderr, "bind: %s", strerror ( errno ) );
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
        fprintf ( stderr, "getaddrinfo: %s", strerror ( e ) );
        return false;
    }
    if ( added_count > 0 )
    {
        r = true;
    }
    return r;
}


us_reactor_handler_t *us_reactor_handler_tcp_server_create ( void )
{
    return calloc ( sizeof ( us_reactor_handler_tcp_server_t ), 1 );
}

bool us_reactor_handler_tcp_server_init (
    us_reactor_handler_t *self_,
    int fd,
    void *client_extra,
    us_reactor_handler_create_proc_t client_handler_create,
    us_reactor_handler_init_proc_t client_handler_init
)
{
    us_reactor_handler_tcp_server_t *self = ( us_reactor_handler_tcp_server_t * ) self_;
    if ( us_reactor_handler_init ( &self->base, fd, client_extra ) )
    {
        self->client_handler_init = client_handler_init;
        self->client_handler_create = client_handler_create;
        self->base.readable = us_reactor_handler_tcp_server_readable;
        self->base.writable = 0;
        self->base.wake_on_readable = true;
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
    int accepted_fd = accept ( self->base.fd, ( struct sockaddr * ) &rem, &remlen );
    if ( accepted_fd != -1 )
    {
        us_reactor_handler_t *client_item = self->client_handler_create();
        if ( client_item != NULL )
        {
            r = self->client_handler_init ( client_item, accepted_fd, self->base.extra );
            if ( r )
            {
                us_reactor_handler_tcp_t * tcp_item = ( us_reactor_handler_tcp_t * ) client_item;
                r = self->base.reactor->add_item ( self->base.reactor, client_item );
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
                closesocket ( client_item->fd );
                client_item->destroy ( client_item );
                free ( client_item );
            }
        }
    }
    return r;
}


us_reactor_handler_t * us_reactor_handler_tcp_create ( void )
{
    return calloc ( sizeof ( us_reactor_handler_tcp_t ), 1 );
}

bool us_reactor_handler_tcp_init (
    us_reactor_handler_t *self_,
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
    if ( us_reactor_handler_init ( self_, fd, extra ) )
    {
        self->base.destroy = us_reactor_handler_tcp_destroy;
        self->base.tick = us_reactor_handler_tcp_tick;
        self->base.readable = us_reactor_handler_tcp_readable;
        self->base.writable = us_reactor_handler_tcp_writable;
        self->base.wake_on_readable = true;
        self->xfer_buf_size = xfer_buf_size;
        in_buf = ( uint8_t * ) calloc ( queue_buf_size, 1 );
        out_buf = ( uint8_t * ) calloc ( queue_buf_size, 1 );
        self->xfer_buf = ( char * ) calloc ( xfer_buf_size, 1 );
        self->connected = 0;
        self->readable = 0;
        self->tick = 0;
    }
    if ( in_buf != NULL && out_buf != NULL && self->xfer_buf != NULL )
    {
        us_queue_init ( &self->incoming_queue, in_buf, queue_buf_size );
        us_queue_init ( &self->outgoing_queue, out_buf, queue_buf_size );
        r = true;
    }
    else
    {
        self->base.destroy ( &self->base );
        r = false;
    }
    return r;
}

void us_reactor_handler_tcp_destroy (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    free ( self->incoming_queue.m_buf );
    free ( self->outgoing_queue.m_buf );
    free ( self->xfer_buf );
    us_reactor_handler_destroy ( &self->base );
}

bool us_reactor_handler_tcp_tick (
    us_reactor_handler_t *self_
)
{
    bool r = true;
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    if ( us_queue_writable_count ( &self->incoming_queue ) >= self->xfer_buf_size )
    {
        self->base.wake_on_readable = true;
    }
    else
    {
        self->base.wake_on_readable = false;
    }
    if ( us_queue_can_read_byte ( &self->outgoing_queue ) )
    {
        self->base.wake_on_writable = true;
    }
    else
    {
        self->base.wake_on_writable = false;
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
    len = recv ( self->base.fd, self->xfer_buf, self->xfer_buf_size, 0 );
    if ( len > 0 )
    {
        us_queue_write ( &self->incoming_queue, ( uint8_t* ) self->xfer_buf, len );
        if ( self->readable )
        {
            self->readable ( self );
        }
    }
    else
    {
        closesocket ( self->base.fd );
        self->base.fd = -1;
    }
    return true;
}

bool us_reactor_handler_tcp_writable (
    us_reactor_handler_t *self_
)
{
    us_reactor_handler_tcp_t *self = ( us_reactor_handler_tcp_t * ) self_;
    bool r = false;
    int len;
    len = us_queue_readable_count ( &self->outgoing_queue );
    if ( len > 0 )
    {
        uint8_t *outgoing = us_queue_contig_read_ptr ( &self->outgoing_queue );
        int outgoing_len = us_queue_contig_readable_count ( &self->outgoing_queue );
        len = send ( self->base.fd, (const char *)outgoing, outgoing_len, 0 );
        if ( len > 0 )
        {
            us_queue_skip ( &self->outgoing_queue, len );
            r = true;
        }
    }
    if ( r == false )
    {
        closesocket ( self->base.fd );
        self->base.fd = -1;
    }
    return r;
}


