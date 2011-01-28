
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

#include "us_webapp_fs.h"

us_webapp_file_ext_mime_map_t us_webapp_fs_default_ext_map[] =
{
    { ".txt", "text/plain; charset=utf-8" },
    { ".html", "text/html; charset=utf-8" },
    { ".htm", "text/html; charset=utf-8" },
    { ".js", "application/javascript" },
    { ".jpg", "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".png", "image/png" },
    { ".gif", "image/gif" },
    { ".ico", "image/x-icon" },
    { ".css", "text/css" },
    { 0, 0 }
};

static const char * us_webapp_file_ext_mime_map_find( us_webapp_file_ext_mime_map_t *table, const char *path )
{
    const char *content_type = "application/octet-stream";
    const char *last_dot = strrchr( path, '.' );
    const char *last_slash = strchr( path, '/' );
    if( last_dot > last_slash )
    {
        us_webapp_file_ext_mime_map_t *cur=table;
        while( cur && cur->m_extension && cur->m_mime_type )
        {
            if( strcmp( cur->m_extension, last_dot )==0 )
            {
                content_type=cur->m_mime_type;
                break;
            }
            cur++;
        }
    }
    return content_type;
}

#if US_ENABLE_STDIO

us_webapp_t *us_webapp_fs_create(
        us_allocator_t *allocator,
        us_webapp_file_ext_mime_map_t *ext_map,
        const char *web_path_prefix,
        const char *filesystem_path
        )
{
    us_webapp_fs_t *self = us_new( allocator, us_webapp_fs_t );
    if( self )
    {
        if( !us_webapp_init( &self->m_base, allocator ) )
        {
            us_webapp_destroy(&self->m_base);
            self=0;
        }
    }
    if( self )
    {
        self->m_base.destroy = us_webapp_fs_destroy;
        self->m_base.dispatch = us_webapp_fs_dispatch;
        self->m_base.path_match = us_webapp_fs_path_match;
        self->m_web_path_prefix_len = strlen( web_path_prefix );
        self->m_filesystem_path = us_strdup( allocator, filesystem_path );
        self->m_web_path_prefix = us_strdup( allocator, web_path_prefix );
        if( ext_map )
        {
            self->m_ext_map = ext_map;
        }
        else
        {
            self->m_ext_map = us_webapp_fs_default_ext_map;
        }
        if( self->m_filesystem_path==0 || self->m_web_path_prefix==0 )
        {
            self->m_base.destroy( &self->m_base );
            self=0;
        }
    }
    return &self->m_base;
}


void us_webapp_fs_destroy(
    us_webapp_t *self_
)
{
    us_webapp_fs_t *self = (us_webapp_fs_t *)self_;
    us_delete( self->m_base.m_allocator, self->m_filesystem_path );
    us_delete( self->m_base.m_allocator, self->m_web_path_prefix );
    us_webapp_destroy( self_ );
}


bool us_webapp_fs_path_match(us_webapp_t *self_, const char *path )
{
    bool r=false;
    us_webapp_fs_t *self = (us_webapp_fs_t *)self_;
    if( strncmp( path, self->m_web_path_prefix, self->m_web_path_prefix_len )==0 )
    {
        r=true;
    }
    return r;
}


static bool us_webapp_fs_validate_path( const char *path )
{
    bool r=true;
    bool last_was_dot=false;
    int i;
    int len = strlen( path );

    for( i=0; i<len; ++i )
    {
        char c= path[i];
        if( c=='.' && last_was_dot )
        {
            r=false;
            break;
        }
        if( c=='.')
        {
            last_was_dot=true;
        }
        else
        {
            last_was_dot=false;
        }
        if( !isprint(c) || (c&0x80)!=0 )
        {
            r=false;
            break;
        }
    }
    return r;
}

static bool us_webapp_fs_read_file( us_buffer_t *buf, const char *fs_path, const char *file_path )
{
    FILE *f;
    bool r=false;
    char full_path[4096];

    if( us_strncpy( full_path, fs_path, sizeof(full_path) ) &&
        us_strncat( full_path, file_path, sizeof(full_path) ) )
    {
        f=fopen( full_path, "rb" );
        if( f )
        {
            int32_t file_len=0;
            fseek(f,0,SEEK_END);
            file_len = ftell(f);
            if( file_len>=0 )
            {
                fseek(f,0,SEEK_SET);
                if( file_len<=buf->m_max_length )
                {
                    if( file_len>0 )
                    {
                        if( fread( buf->m_buffer, file_len, 1, f)==1 )
                        {
                            buf->m_cur_length = file_len;
                            r=true;
                        }
                        else
                        {
                            us_log_error( "webapp_fs file '%s' unable to be read" );
                        }
                    }
                    else
                    {
                        r=true;
                    }
                }
                else
                {
                    /* too big */
                    us_log_error( "webapp_fs file '%s' is too big, %ld bytes > buffer %ld bytes",
                                  full_path, file_len, buf->m_max_length
                                  );
                    r=false;
                }
            }
            fclose(f);
        }
    }
    return r;
}


int us_webapp_fs_dispatch(
    us_webapp_t *self_,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    us_webapp_fs_t *self = (us_webapp_fs_t *)self_;
    bool r=true;
    bool is_head=(strcmp( request_header->m_method, "HEAD")==0);
    bool is_get=(strcmp( request_header->m_method, "GET")==0);
    const char *content_type = "application/octet-stream";
    (void)request_content;

    /* only try handle HEAD and GET requests */
    if( !is_head && !is_get )
    {
        us_http_response_header_init_error( response_header, 404, 0, 0 );
        us_log_debug( "invalid request method '%s'' for fs path '%s'", request_header->m_method, request_header->m_path );
        return 404;
    }

    /* validate that the path only contains safe characters */
    r=us_webapp_fs_validate_path( request_header->m_path );
    if( !r )
    {
        us_http_response_header_init_error( response_header, 400, 0, 0 );
        us_log_debug( "invalid characters in path '%s'",request_header->m_path );
        return 400;
    }

    /* path ends in / means readirect to index.html */
    {
        int len = strlen( request_header->m_path );
        if( len>0 && request_header->m_path[len-1]=='/' )
        {
            char new_path[4096];
            if(
                us_strncpy( new_path, request_header->m_path, sizeof(new_path)  ) &&
                us_strncat( new_path, "index.html", sizeof( new_path) ) )
            {
                r=us_http_response_header_init_redirect(response_header,302,new_path);
            }
            else
            {
                r=false;
            }
            if( !r )
            {
                response_header->m_code=500;
            }
            return response_header->m_code;
        }
    }

    /* try open the file */
    r=us_webapp_fs_read_file( response_content, self->m_filesystem_path, request_header->m_path );

    if( !r )
    {
        us_http_response_header_init_error( response_header, 404, 0, 0 );
        us_log_debug( "unable to open file in '%s' for path '%s'",self->m_filesystem_path, request_header->m_path );
        return 404;
    }

    content_type=us_webapp_file_ext_mime_map_find( self->m_ext_map, request_header->m_path );

    r&=us_http_response_header_set_content_type ( response_header, content_type );
    r&=us_http_response_header_set_connection_close ( response_header );
    r&=us_http_response_header_set_content_length ( response_header, response_content->m_cur_length );

    if( r )
    {
        response_header->m_code = 200;
    }
    else
    {
        us_log_error( "unable to fill response header, replying with 500");
        us_http_response_header_init_error( response_header, 500, 0, 0 );
    }

    return response_header->m_code;
}

#endif
