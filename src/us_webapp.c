
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

#include "us_webapp.h"

bool us_webapp_init( us_webapp_t *self, us_allocator_t *allocator )
{
    self->m_allocator = allocator;
    self->destroy = us_webapp_destroy;
    self->path_match = us_webapp_path_match;
    self->dispatch = us_webapp_dispatch;
    return true;
}

void us_webapp_destroy( us_webapp_t *self )
{
}

bool us_webapp_path_match(us_webapp_t *self, const char *path )
{
    return true;
}


int us_webapp_dispatch(
    us_webapp_t *self,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    return -1;
}


bool us_webapp_director_init( us_webapp_director_t *self, us_allocator_t *allocator )
{
    self->destroy = us_webapp_director_destroy;
    self->dispatch = us_webapp_director_dispatch;
    self->m_allocator = allocator;
    self->m_apps = 0;
    self->m_last_app = 0;
    /* TODO: */
    return true;
}


void us_webapp_director_destroy( us_webapp_director_t *self )
{
    us_webapp_t *cur = self->m_apps;
    while( cur )
    {
        us_webapp_t *next = cur->m_next;
        cur->destroy( cur );
        cur = next;
    }
}


bool us_webapp_director_add_app( us_webapp_director_t *self, us_webapp_t *app )
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

int us_webapp_director_dispatch(
    us_webapp_director_t *self,
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
    us_buffer_append_string(response_content, "\r\nContent is:\r\n" );
    us_buffer_append_data( response_content, request_content->m_buffer, request_content->m_cur_length );
    response_header->m_code = 200;
    if ( !us_http_response_header_set_content_type ( response_header, "text/plain" ) ||
            !us_http_response_header_set_content_length ( response_header, response_content->m_cur_length ) ||
            !us_http_response_header_set_connection_close ( response_header )  )
    {
        return -1;
    }
    return response_header->m_code;
}

