
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

#include "us_world.h"

#include "us_http_server.h"

bool us_http_server_app_init( us_http_server_app_t *self, us_allocator_t *allocator )
{
    self->m_allocator = allocator;
    self->destroy = us_http_server_app_destroy;
    self->path_match = us_http_server_app_path_match;
    self->dispatch = us_http_server_app_dispatch;
    return true;
}

void us_http_server_app_destroy( us_http_server_app_t *self )
{
}

bool us_http_server_app_path_match(us_http_server_app_t *self, const char *path )
{
    return true;
}


int us_http_server_app_dispatch(
    us_http_server_app_t *self,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    return -1;
}


bool us_http_server_director_init( us_http_server_director_t *self, us_allocator_t *allocator )
{
    self->destroy = us_http_server_director_destroy;
    self->dispatch = us_http_server_director_dispatch;
    self->m_allocator = allocator;
    self->m_apps = 0;
    self->m_last_app = 0;
    /* TODO: */
    return true;
}


void us_http_server_director_destroy( us_http_server_director_t *self )
{
    us_http_server_app_t *cur = self->m_apps;
    while( cur )
    {
        us_http_server_app_t *next = cur->m_next;
        cur->destroy( cur );
        cur = next;
    }
}


bool us_http_server_director_add_app( us_http_server_director_t *self, us_http_server_app_t *app )
{
    if( self->m_last_app )
    {
        self->m_last_app->m_next = app;
        self->m_last_app = app;
    }
    else
    {
        self->m_apps =app;
        self->m_last_app = app;
    }
    return true;
}

int us_http_server_director_dispatch(
    us_http_server_director_t *self,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    us_http_header_item_t *item=0;
    /* TODO: go through app list, find one that matches the path and dispatch to it */
    us_buffer_append_string(response_content, "Request Info\r\nMethod='" );
    us_buffer_append_string(response_content, request_header->m_method );
    us_buffer_append_string(response_content, "'\r\nPath='" );
    us_buffer_append_string(response_content, request_header->m_path );
    us_buffer_append_string(response_content, "'\r\nVersion='" );
    us_buffer_append_string(response_content, request_header->m_version );
    us_buffer_append_string(response_content, "'\r\n\r\n" );
    item=request_header->m_items->m_first;
    while( item )
    {
        us_buffer_append_string(response_content,item->m_key);
        us_buffer_append_string(response_content,": ");
        us_buffer_append_string(response_content,item->m_value);
        us_buffer_append_string(response_content,"\r\n");
        item=item->m_next;
    }
    response_header->m_code = 200;
    if ( !us_http_response_header_set_content_type ( response_header, "text/plain" ) ||
            !us_http_response_header_set_content_length ( response_header, response_content->m_cur_length ) )
    {
        return -1;
    }
    return response_header->m_code;
}

us_reactor_handler_t * us_http_server_handler_create(us_allocator_t *allocator)
{
    return (us_reactor_handler_t *)us_new( allocator, us_http_server_handler_t );
}

bool us_http_server_handler_init(
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra,
    int32_t max_request_buffer_size,
    int32_t max_response_buffer_size,
    us_http_server_director_t *director
)
{
    bool r=false;
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    r = us_reactor_handler_tcp_init(
            &self->m_base.m_base,
            allocator,
            fd,
            extra,
            US_HTTP_SERVER_HANDLER_REQUEST_HEADER_SIZE + max_request_buffer_size,
            US_HTTP_SERVER_HANDLER_RESPONSE_HEADER_SIZE + max_response_buffer_size
        );
    if( r )
    {
        self->m_local_allocator_buffer = us_new_array(
                                             allocator,
                                             char,
                                             US_HTTP_SERVER_HANDLER_LOCAL_BUFFER_SIZE + max_response_buffer_size
                                         );
        self->m_request_header = 0;
        self->m_response_header = 0;
        if( self->m_local_allocator_buffer )
        {
            us_simple_allocator_init( &self->m_local_allocator, self->m_local_allocator_buffer, US_HTTP_SERVER_HANDLER_LOCAL_BUFFER_SIZE );
            self->m_request_header = us_http_request_header_create( &self->m_local_allocator.m_base );
            self->m_response_header = us_http_response_header_create( &self->m_local_allocator.m_base );
            self->m_response_content = us_buffer_create(&self->m_local_allocator.m_base, max_response_buffer_size );
            self->m_director = director;
            us_http_server_handler_set_state_waiting_for_connection( self );
#ifdef __linux__
            {
                int arg = 2; /* seconds when idle */
                if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &arg, sizeof(arg)) != 0)
                {
                    us_log_error( "unable to set TCP_KEEPIDLE" );
                }
                arg = 2; /* seconds when not idle */
                if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &arg, sizeof(arg)) != 0)
                {
                    us_log_error( "unable to set TCP_KEEPINTVL" );
                }
                arg = 2; /* max probe count before closing socket */
                if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &arg, sizeof(arg)) != 0)
                {
                    us_log_error( "unable to set TCP_KEEPCNT" );
                }
                arg = 1; /* enable keepalive */
                if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &arg, sizeof(arg)) != 0)
                {
                    us_log_error( "unable to set SO_KEEPALIVE" );
                }
            }
#endif
            r=true;
        }
    }
    return r;
}

void us_http_server_handler_destroy(
    us_reactor_handler_t *self_
)
{
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    if( self->m_request_header )
    {
        self->m_request_header->destroy( self->m_request_header );
    }
    if( self->m_response_header )
    {
        self->m_response_header->destroy( self->m_response_header );
    }
    if( self->m_response_content )
    {
        self->m_response_content->destroy( self->m_response_content );
    }
    if( self->m_local_allocator_buffer )
    {
        self->m_local_allocator.m_base.destroy( &self->m_local_allocator.m_base );
        self->m_base.m_base.m_allocator->free( self->m_base.m_base.m_allocator, self->m_local_allocator_buffer );
    }
    us_reactor_handler_tcp_destroy(&self->m_base.m_base);
}


bool us_http_server_handler_connected(
    us_reactor_handler_tcp_t *self_,
    struct sockaddr *addr,
    socklen_t addrlen
)
{
    /* someone made a tcp connection to us, clear our buffers, allocators, and parsers */
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    if( us_log_level >= US_LOG_LEVEL_DEBUG && addr )
    {
        char hostname_buf[1024];
        char srv_buf[64];
        if( getnameinfo( addr, addrlen, hostname_buf, sizeof(hostname_buf), srv_buf, sizeof(srv_buf), NI_NUMERICHOST | NI_NUMERICSERV )==0 )
        {
            us_log_debug( "accepted connection from '%s:%s'", hostname_buf, srv_buf );
        }
        else
        {
            us_log_debug( "accepted connection, unable to resolve adress" );
        }
    }
    /* we are now in receiving request header */
    us_http_server_handler_set_state_receiving_request_header( self );
    return true;
}

bool us_http_server_handler_readable_request_header(
    us_reactor_handler_tcp_t *self_
)
{
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    bool r=true;
    us_queue_t *incoming = &self->m_base.m_incoming_queue;
    int i;
    bool found_end_of_header=false;
    /* scan until "\r\n\r\n" is seen in the incoming queue */
    for( i=self->m_byte_count; i<us_queue_readable_count(incoming)-3; i++ )
    {
        if( us_queue_peek( incoming, i )=='\r' &&
                us_queue_peek( incoming, i+1 )=='\n' &&
                us_queue_peek( incoming, i+2 )=='\r' &&
                us_queue_peek( incoming, i+3 )=='\n' )
        {
            found_end_of_header=true;
            /* skip over the request header that we parsed */
            us_queue_skip( incoming, i+4 );
            break;
        }
    }
    if( found_end_of_header )
    {
        /* found the end of the header, try parse it */
        r=us_http_server_handler_parse_request_header(self);
        /* if parsing fails, r=false means that the socket will close */
    }
    else
    {
        /* next time, continue scanning where we left off */
        self->m_byte_count = i;
    }
    return r;
}


bool us_http_server_handler_parse_request_header(
    us_http_server_handler_t *self
)
{
    bool r=false;
    us_buffer_t incoming_buffer;
    us_buffer_init(
        &incoming_buffer,
        0,
        self->m_base.m_incoming_queue.m_buf,
        self->m_base.m_incoming_queue.m_next_in
    );
    incoming_buffer.m_cur_length = incoming_buffer.m_max_length;
    r=us_http_request_header_parse(
          self->m_request_header,
          &incoming_buffer
      );
    if( r )
    {
        /* we have parsed a request! */
        /* TODO: see if http version is 1.1 or higher */
        /* bool http_vesion_1_1 = us_http_request_header_get_http_version( self->m_request_header )>11 */
        /* TODO: try find Connection: Close */
        /* bool connection_close = us_http_request_header_get_connection_close( self->m_request_header ); */
        /* TODO: try find Connetion: Keep-Alive */
        /* bool connection_keep_alive = us_http_request_header_get_connection_keep_alive */
        /* TODO: try find the expected content length */
        /* Get Content-Length field, if there is one */
        us_http_header_item_t *content_length_item;
        content_length_item = self->m_request_header->m_items->find( self->m_request_header->m_items, "Content-Length" );
        if( content_length_item )
        {
            /* found it, extract the value as our todo count */
            self->m_todo_count = strtol( content_length_item->m_value, 0, 10 );
        }
        /* Some verbs might have content even if content length field is nonexistant */
        if( strcmp( self->m_request_header->m_method, "POST" )==0 ||
                strcmp( self->m_request_header->m_method, "PUT" )==0 )
        {
            /* if we don't have a content_length item, by default todo_count is -1 meaning 'wait for close' */
            if( !content_length_item )
            {
                self->m_todo_count=-1;
            }
        }
        else
        {
            /* any other verb by default has no content if content-length is missing */
            if( !content_length_item )
            {
                self->m_todo_count = 0;
            }
        }
        /* now, if we have a non-zero content-length to exepect, either -1 or >0, then we go to receive content state */
        /* otherwise, we go straight to handling the request immediately */
        if( self->m_todo_count!=0 )
        {
            us_http_server_handler_set_state_receiving_request_content( self );
            r=true;
        }
        else
        {
            r=us_http_server_handler_dispatch( self );
        }
    }
    return r;
}

bool us_http_server_handler_dispatch(
    us_http_server_handler_t *self
)
{
    bool r=false;
    us_queue_t *incoming = &self->m_base.m_incoming_queue;
    us_queue_t *outgoing = &self->m_base.m_outgoing_queue;
    us_buffer_t request_content;
    us_buffer_t response_header_buffer;
    us_buffer_init(
        &request_content,
        0,
        incoming->m_buf + incoming->m_next_out,
        us_queue_readable_count( incoming )
    );
    request_content.m_cur_length = request_content.m_max_length;
    if( self->m_director->dispatch(
                self->m_director,
                self->m_request_header,
                &request_content,
                self->m_response_header,
                self->m_response_content
            )>=0 )
    {
        us_buffer_init(
            &response_header_buffer,
            0,
            outgoing->m_buf,
            outgoing->m_buf_size
        );
        r=us_http_response_header_flatten(
              self->m_response_header,
              &response_header_buffer
          );
        if( r )
        {
            outgoing->m_next_in = response_header_buffer.m_cur_length;
            if( us_queue_writable_count( outgoing )>self->m_response_content->m_cur_length )
            {
                us_queue_write(outgoing,self->m_response_content->m_buffer,self->m_response_content->m_cur_length);
                us_http_server_handler_set_state_sending_response(self);
                /* response header and response content is now in the outgoing buffer */
                r=true;
            }
            else
            {
                r=false;
            }
        }
    }
    return r;
}

bool us_http_server_handler_readable_request_content(
    us_reactor_handler_tcp_t *self_
)
{
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    bool r=true;
    us_queue_t *incoming = &self->m_base.m_incoming_queue;
    /* if content length is -1, we read until close */
    /* if content length is non zero, we read until incoming queue contains that much */
    if( self->m_todo_count == -1 )
    {
        r=true;
    }
    else if( us_queue_readable_count(incoming) >= self->m_todo_count )
    {
        r=us_http_server_handler_dispatch( self );
    }
    return r;
}

bool us_http_server_handler_eof_request_content(
    us_reactor_handler_tcp_t *self_
)
{
    us_http_server_handler_t *self = (us_http_server_handler_t *)self_;
    bool r=false;
    if( self->m_todo_count ==-1 )
    {
        /* when content length is unknown then eof means end of request content, time to dispatch request */
        r=us_http_server_handler_dispatch( self );
    }
    return r;
}


bool us_http_server_handler_writable_response(
    us_reactor_handler_tcp_t *self_
)
{
    /* once the entire response is sent, close the socket */
    return false;
}

bool us_http_server_handler_eof_response(
    us_reactor_handler_tcp_t *self_
)
{
    bool r=true;
    /* eof on incoming data while sending response header is to be ignored */
    return r;
}





void us_http_server_handler_closed(
    us_reactor_handler_tcp_t *self_
)
{
    /* nothing to do here */
}
