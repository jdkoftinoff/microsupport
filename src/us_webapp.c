
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

void us_webapp_destroy( us_webapp_t *US_UNUSED(self) )
{
}

bool us_webapp_path_match(us_webapp_t *US_UNUSED(self), const char *US_UNUSED(path) )
{
    return true;
}


int us_webapp_dispatch(
    us_webapp_t *US_UNUSED(self),
    const us_http_request_header_t *US_UNUSED(request_header),
    const us_buffer_t *US_UNUSED(request_content),
    us_http_response_header_t *US_UNUSED(response_header),
    us_buffer_t *US_UNUSED(response_content)
)
{
    return -1;
}


us_webapp_t *us_webapp_static_buffer_create(
    us_allocator_t *allocator
)
{
    us_webapp_static_buffer_t *self = us_new( allocator, us_webapp_static_buffer_t );
    if ( self )
    {
        if ( !us_webapp_init( &self->m_base, allocator ) )
        {
            us_webapp_destroy(&self->m_base);
            self=0;
        }
    }
    if ( self )
    {
        self->m_base.destroy = us_webapp_static_buffer_destroy;
        self->m_base.dispatch = us_webapp_static_buffer_dispatch;
        self->m_base.path_match = us_webapp_static_buffer_path_match;
        self->m_path = 0;
        self->m_buffer = 0;
        self->m_content_type = 0;
        self->m_owned_buffer = false;
    }
    return &self->m_base;
}

us_webapp_t *us_webapp_static_buffer_create_with_buffer(
    us_allocator_t *allocator,
    const char *path,
    const char *content_type,
    us_buffer_t *buffer,
    bool own_buffer
)
{
    us_webapp_static_buffer_t *self = (us_webapp_static_buffer_t *)us_webapp_static_buffer_create( allocator );
    if ( self )
    {
        self->m_path = us_strdup( self->m_base.m_allocator, path );
        self->m_content_type = us_strdup( self->m_base.m_allocator, content_type );
        self->m_buffer = buffer;
        self->m_owned_buffer = own_buffer;
        if ( !self->m_path || !self->m_content_type )
        {
            us_webapp_static_buffer_destroy( &self->m_base );
            self = 0;
        }
    }
    return &self->m_base;
}

us_webapp_t *us_webapp_static_buffer_create_with_string(
    us_allocator_t *allocator,
    const char *US_UNUSED(path),
    const char *content_type,
    const char *str
)
{
    us_webapp_static_buffer_t *self=0;
    us_buffer_t *buf = us_buffer_create(allocator,strlen(str));
    if ( buf )
    {
        self = (us_webapp_static_buffer_t *)us_webapp_static_buffer_create( allocator );
        if ( self )
        {
            self->m_content_type = us_strdup( self->m_base.m_allocator, content_type );
            if ( self->m_content_type )
            {
                us_buffer_append_string(buf,str);
                self->m_buffer = buf;
                self->m_owned_buffer = true;
            }
            else
            {
                self->m_base.destroy( &self->m_base );
            }
        }
        else
        {
            buf->destroy( buf );
        }
    }
    return &self->m_base;
}

void us_webapp_static_buffer_destroy(
    us_webapp_t *self_
)
{
    us_webapp_static_buffer_t *self = (us_webapp_static_buffer_t *)self_;
    if ( self->m_owned_buffer )
    {
        if ( self->m_buffer )
        {
            self->m_buffer->destroy( self->m_buffer );
        }
    }
    us_delete( self->m_base.m_allocator, self->m_path );
    us_delete( self->m_base.m_allocator, self->m_content_type );
    us_webapp_destroy( self_ );
}


bool us_webapp_static_buffer_path_match(us_webapp_t *self_, const char *path )
{
    bool r=false;
    us_webapp_static_buffer_t *self = (us_webapp_static_buffer_t *)self_;
    if ( strcmp( path, self->m_path )==0 )
    {
        r=true;
    }
    return r;
}

int us_webapp_static_buffer_dispatch(
    us_webapp_t *self_,
    const us_http_request_header_t *US_UNUSED(request_header),
    const us_buffer_t *US_UNUSED(request_content),
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    us_webapp_static_buffer_t *self = (us_webapp_static_buffer_t *)self_;
    bool r=true;
    if ( self->m_buffer )
    {
        r&=us_buffer_write_buffer(response_content, self->m_buffer );
    }
    if ( self->m_content_type )
    {
        r&=us_http_response_header_set_content_type ( response_header, self->m_content_type );
    }
    r&=us_http_response_header_set_connection_close ( response_header );
    r&=us_http_response_header_set_content_length ( response_header, (int)us_buffer_readable_count( response_content ) );
    if ( r )
    {
        response_header->m_code = 200;
    }
    else
    {
        response_header->m_code = -1;
    }
    return response_header->m_code;
}

us_webapp_t *us_webapp_redirect_create(
    us_allocator_t *allocator,
    const char *original_path,
    const char *new_path,
    int redirect_code
)
{
    us_webapp_redirect_t *self = us_new( allocator, us_webapp_redirect_t );
    if ( self )
    {
        if ( !us_webapp_init( &self->m_base, allocator ) )
        {
            us_webapp_destroy(&self->m_base);
            self=0;
        }
    }
    if ( self )
    {
        self->m_base.destroy = us_webapp_redirect_destroy;
        self->m_base.dispatch = us_webapp_redirect_dispatch;
        self->m_base.path_match = us_webapp_redirect_path_match;
        self->m_original_path = us_strdup( allocator, original_path );
        self->m_new_path = us_strdup( allocator, new_path );
        self->m_redirect_code = redirect_code;
        if ( !self->m_original_path || !self->m_new_path )
        {
            self->m_base.destroy( &self->m_base );
            self=0;
        }
    }
    return &self->m_base;
}

void us_webapp_redirect_destroy(
    us_webapp_t *self_
)
{
    us_webapp_redirect_t *self = (us_webapp_redirect_t *)self_;
    us_delete( self->m_base.m_allocator, self->m_original_path );
    us_delete( self->m_base.m_allocator, self->m_new_path );
    us_webapp_destroy( self_ );
}

bool us_webapp_redirect_path_match(us_webapp_t *self_, const char *path )
{
    bool r=false;
    us_webapp_redirect_t *self = (us_webapp_redirect_t *)self_;
    if ( strcmp( path, self->m_original_path )==0 )
    {
        r=true;
    }
    return r;
}

int us_webapp_redirect_dispatch(
    us_webapp_t *self_,
    const us_http_request_header_t *US_UNUSED(request_header),
    const us_buffer_t *US_UNUSED(request_content),
    us_http_response_header_t *response_header,
    us_buffer_t *US_UNUSED(response_content)
)
{
    us_webapp_redirect_t *self = (us_webapp_redirect_t *)self_;
    us_http_response_header_init_redirect( response_header, self->m_redirect_code, self->m_new_path );
    return response_header->m_code;
}



us_webapp_t *us_webapp_diag_create(
    us_allocator_t *allocator
)
{
    us_webapp_diag_t *self = us_new( allocator, us_webapp_diag_t );
    if ( self )
    {
        if ( !us_webapp_init( &self->m_base, allocator ) )
        {
            us_webapp_destroy(&self->m_base);
            self=0;
        }
    }
    if ( self )
    {
        self->m_base.destroy = us_webapp_diag_destroy;
        self->m_base.dispatch = us_webapp_diag_dispatch;
        self->m_base.path_match = us_webapp_diag_path_match;
    }
    return &self->m_base;
}

void us_webapp_diag_destroy(
    us_webapp_t *self
)
{
    us_webapp_destroy(self);
}

bool us_webapp_diag_path_match(us_webapp_t *US_UNUSED(self), const char *US_UNUSED(url) )
{
    return true;
}

int us_webapp_diag_dispatch(
    us_webapp_t *US_UNUSED(self),
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    us_http_header_item_t *item=0;
    us_buffer_append_string(response_content, "Request Info\r\nMethod='" );
    us_buffer_append_string(response_content, request_header->m_method );
    us_buffer_append_string(response_content, "'\r\nPath='" );
    us_buffer_append_string(response_content, request_header->m_path );
    us_buffer_append_string(response_content, "'\r\nVersion='" );
    us_buffer_append_string(response_content, request_header->m_version );
    us_buffer_append_string(response_content, "'\r\n\r\n" );
    item=request_header->m_items->m_first;
    while ( item )
    {
        us_buffer_append_string(response_content,item->m_key);
        us_buffer_append_string(response_content,": ");
        us_buffer_append_string(response_content,item->m_value);
        us_buffer_append_string(response_content,"\r\n");
        item=item->m_next;
    }
    us_buffer_append_string(response_content, "\r\nContent is:\r\n" );
    us_buffer_write_buffer( response_content, request_content );
    response_header->m_code = 200;
    if ( !us_http_response_header_set_content_type ( response_header, "text/plain" ) ||
            !us_http_response_header_set_content_length ( response_header, (int)us_buffer_readable_count(response_content) ) ||
            !us_http_response_header_set_connection_close ( response_header )  )
    {
        return -1;
    }
    return response_header->m_code;
}


bool us_webapp_director_init( us_webapp_director_t *self, us_allocator_t *allocator )
{
    self->destroy = us_webapp_director_destroy;
    self->dispatch = us_webapp_director_dispatch;
    self->m_allocator = allocator;
    self->m_apps = 0;
    self->m_last_app = 0;
    self->m_404_app = 0;
    return true;
}


void us_webapp_director_destroy( us_webapp_director_t *self )
{
    us_webapp_t *cur = self->m_apps;
    while ( cur )
    {
        us_webapp_t *next = cur->m_next;
        cur->destroy( cur );
        cur = next;
    }
    if ( self->m_404_app )
    {
        self->m_404_app->destroy( self->m_404_app );
    }
}


bool us_webapp_director_add_app( us_webapp_director_t *self, us_webapp_t *app )
{
    bool r=false;
    if ( app )
    {
        if ( self->m_last_app )
        {
            self->m_last_app->m_next = app;
            self->m_last_app = app;
        }
        else
        {
            self->m_apps =app;
            self->m_last_app = app;
        }
        r=true;
    }
    return r;
}

bool us_webapp_director_add_404_app( us_webapp_director_t *self, us_webapp_t *m_app )
{
    self->m_404_app = m_app;
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
    int http_code=-1;
    us_webapp_t *cur = self->m_apps;
    /* TODO: convert path '%' escape codes and '+' into ascii */
    while ( cur && http_code==-1 )
    {
        if ( cur->path_match( cur, request_header->m_path ) )
        {
            http_code = cur->dispatch(
                            cur,
                            request_header,
                            request_content,
                            response_header,
                            response_content
                        );
        }
        if ( http_code==-1 )
        {
            cur = cur->m_next;
        }
    }
    /* if http_code is still -1 or 404 then no one handled it, pass it to 404 */
    if ( http_code==-1 || http_code==404 )
    {
        if ( self->m_404_app )
        {
            http_code = self->m_404_app->dispatch(
                            self->m_404_app,
                            request_header,
                            request_content,
                            response_header,
                            response_content
                        );
        }
        else
        {
            static const char default_404_type[] = "text/html; charset=utf-8";
            static const char default_404[] = "<html><head><title>Error 404: Not Found</title></head><body><h1>Error 404</h1><p>Not Found</p></body></html>";
            us_http_response_header_init_error(
                response_header, 404,
                default_404_type,
                sizeof(default_404)
            );
            us_buffer_append_string( response_content, default_404 );
        }
    }
    /* if we STILL have -1, then this is an internal server error */
    if ( http_code==-1 )
    {
        http_code=500;
        us_http_response_header_init_error(
            response_header, http_code, 0, 0
        );
    }
    return http_code;
}

