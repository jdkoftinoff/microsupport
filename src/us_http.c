
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

#include "us_logger_syslog.h"
#include "us_http.h"

us_http_header_item_list_t *
us_http_header_item_list_create ( us_allocator_t *allocator )
{
    us_http_header_item_list_t *self = us_new ( allocator, us_http_header_item_list_t );
    if ( self )
    {
        self->destroy = us_http_header_item_list_destroy;
        self->add = us_http_header_item_list_add;
        self->addn = us_http_header_item_list_addn;
        self->find = us_http_header_item_list_find;
        self->remove = us_http_header_item_list_remove;
        self->m_allocator = allocator;
        self->m_first = 0;
    }
    return self;
}


void us_http_header_item_list_destroy ( us_http_header_item_list_t *self )
{
    us_delete ( self->m_allocator, self );
}

us_http_header_item_t *
us_http_header_item_list_add (
    us_http_header_item_list_t *self,
    const char *key,
    const char *value
)
{
    us_http_header_item_t *item;
    item = us_new ( self->m_allocator, us_http_header_item_t );
    if ( item )
    {
        item->m_key = 0;
        item->m_value = 0;
        item->m_key = us_strdup ( self->m_allocator, key );
        if ( item->m_key )
        {
            item->m_value = us_strdup ( self->m_allocator, value );
        }
        if ( item->m_key == 0 || item->m_value == 0 )
        {
            us_delete ( self->m_allocator, item->m_key );
            us_delete ( self->m_allocator, item->m_value );
            us_delete ( self->m_allocator, item );
            item = 0;
        }
        else
        {
            item->m_next = self->m_first;
            self->m_first = item;
        }
    }
    return item;
}

us_http_header_item_t *
us_http_header_item_list_addn (
    us_http_header_item_list_t *self,
    const char *key,
    int key_len,
    const char *value,
    int value_len
)
{
    us_http_header_item_t *item;
    item = us_new ( self->m_allocator, us_http_header_item_t );
    if ( item )
    {
        item->m_key = 0;
        item->m_value = 0;
        item->m_key = us_strndup ( self->m_allocator, key, key_len );
        if ( item->m_key )
        {
            item->m_value = us_strndup ( self->m_allocator, value, value_len );
        }
        if ( item->m_key == 0 || item->m_value == 0 )
        {
            us_delete ( self->m_allocator, item->m_key );
            us_delete ( self->m_allocator, item->m_value );
            us_delete ( self->m_allocator, item );
            item = 0;
        }
        else
        {
            item->m_next = self->m_first;
            self->m_first = item;
        }
    }
    return item;
}

bool
us_http_header_item_list_remove (
    us_http_header_item_list_t *self,
    us_http_header_item_t *item
)
{
    bool r = false;
    us_http_header_item_t *prev = 0;
    us_http_header_item_t *i = self->m_first;
    while ( i )
    {
        if ( i == item )
        {
            prev->m_next = i->m_next;
            us_delete ( self->m_allocator, i->m_key );
            us_delete ( self->m_allocator, i->m_value );
            us_delete ( self->m_allocator, i );
            r = true;
            break;
        }
        prev = i;
        i = i->m_next;
    };
    return r;
}


us_http_header_item_t *
us_http_header_item_list_find (
    us_http_header_item_list_t *self,
    const char *key
)
{
    us_http_header_item_t *r = 0;
    us_http_header_item_t *i = self->m_first;
    while ( i )
    {
#ifdef _WIN32
        if ( _strnicmp ( i->m_key, key, strlen ( key ) ) == 0 )
#else
        if ( strncasecmp ( i->m_key, key, strlen ( key ) ) == 0 )
#endif
        {
            r = i;
            break;
        }
        i = i->m_next;
    };
    return r;
}




us_http_request_header_t *
us_http_request_header_create ( us_allocator_t *allocator )
{
    us_http_request_header_t *self;
    self = us_new ( allocator, us_http_request_header_t );
    if ( self )
    {
        self->m_allocator = allocator;
        self->destroy = us_http_request_header_destroy;
        self->m_items = us_http_header_item_list_create ( allocator );
        self->m_method = 0;
        self->m_path = 0;
        self->m_version = 0;
        if ( self->m_items == 0 )
        {
            us_delete ( allocator, self );
            self = 0;
        }
    }
    return self;
}



void us_http_request_header_destroy ( us_http_request_header_t *self )
{
    self->m_items->destroy ( self->m_items );
    us_delete ( self->m_allocator, self->m_method );
    us_delete ( self->m_allocator, self->m_version );
    us_delete ( self->m_allocator, self->m_path );
    us_delete ( self->m_allocator, self );
}

bool
us_http_request_header_set_method (
    us_http_request_header_t *self,
    const char *method
)
{
    us_delete ( self->m_allocator, self->m_method );
    self->m_method = us_strdup ( self->m_allocator, method );
    return self->m_method != 0;
}

bool
us_http_request_header_set_methodn (
    us_http_request_header_t *self,
    const char *method,
    int len
)
{
    us_delete ( self->m_allocator, self->m_method );
    self->m_method = us_strndup ( self->m_allocator, method, len );
    return self->m_method != 0;
}

bool
us_http_request_header_set_version (
    us_http_request_header_t *self,
    const char *version
)
{
    us_delete ( self->m_allocator, self->m_version );
    self->m_version = us_strdup ( self->m_allocator, version );
    return self->m_version != 0;
}

bool
us_http_request_header_set_versionn (
    us_http_request_header_t *self,
    const char *version,
    int len
)
{
    us_delete ( self->m_allocator, self->m_version );
    self->m_version = us_strndup ( self->m_allocator, version, len );
    return self->m_version != 0;
}

bool
us_http_request_header_set_path (
    us_http_request_header_t *self,
    const char *path
)
{
    us_delete ( self->m_allocator, self->m_path );
    self->m_path = us_strdup ( self->m_allocator, path );
    return self->m_path != 0;
}

bool
us_http_request_header_set_pathn (
    us_http_request_header_t *self,
    const char *path,
    int len
)
{
    us_delete ( self->m_allocator, self->m_path );
    self->m_path = us_strndup ( self->m_allocator, path, len );
    return self->m_path != 0;
}


us_http_response_header_t *
us_http_response_header_create ( us_allocator_t *allocator )
{
    us_http_response_header_t *self;
    self = us_new ( allocator, us_http_response_header_t );
    if ( self )
    {
        self->m_allocator = allocator;
        self->destroy = us_http_response_header_destroy;
        self->m_code = 0;
        self->m_version = us_strdup ( allocator, "HTTP/1.1" );
        self->m_items = us_http_header_item_list_create ( allocator );
        if ( !self->m_items )
        {
            self->destroy ( self );
            self = 0;
        }
    }
    return self;
}



void us_http_response_header_destroy ( us_http_response_header_t *self )
{
    self->m_items->destroy ( self->m_items );
    us_delete ( self->m_allocator, self->m_version );
    us_delete ( self->m_allocator, self );
}

bool
us_http_response_header_set_version (
    us_http_response_header_t *self,
    const char *version
)
{
    us_delete ( self->m_allocator, self->m_version );
    self->m_version = us_strdup ( self->m_allocator, version );
    return self->m_version != 0;
}

bool
us_http_response_header_set_versionn (
    us_http_response_header_t *self,
    const char *version,
    int len
)
{
    us_delete ( self->m_allocator, self->m_version );
    self->m_version = us_strndup ( self->m_allocator, version, len );
    return self->m_version != 0;
}



us_http_request_header_t *
us_http_request_header_create_helper (
    us_allocator_t *allocator,
    const char *method,
    const char *host,
    const char *path
)
{
    us_http_request_header_t *self;
    self = us_http_request_header_create ( allocator );
    if ( self )
    {
        self->m_method = us_strdup ( allocator, method );
        self->m_path = us_strdup ( allocator, path );
        self->m_version = us_strdup ( allocator, "HTTP/1.1" );
        if ( self->m_path == 0 )
        {
            self->destroy ( self );
            self = 0;
        }
        else
        {
            if ( !self->m_items->add ( self->m_items, "Host", host ) )
            {
                self->destroy ( self );
                self = 0;
            }
        }
    }
    return self;
}


us_http_request_header_t *
us_http_request_header_create_get (
    us_allocator_t *allocator,
    const char *host,
    const char *path
)
{
    return us_http_request_header_create_helper (
               allocator,
               "GET",
               host,
               path
           );
}

us_http_request_header_t *
us_http_request_header_create_delete (
    us_allocator_t *allocator,
    const char *host,
    const char *path
)
{
    return us_http_request_header_create_helper (
               allocator,
               "DELETE",
               host,
               path
           );
}


us_http_request_header_t *
us_http_request_header_create_post (
    us_allocator_t *allocator,
    const char *host,
    const char *path,
    const char *content_type,
    uint32_t content_length
)
{
    return us_http_request_header_create_helper (
               allocator,
               "POST",
               host,
               path
           );
}


us_http_request_header_t *
us_http_request_header_create_put (
    us_allocator_t *allocator,
    const char *host,
    const char *path,
    const char *content_type,
    uint32_t content_length
)
{
    return us_http_request_header_create_helper (
               allocator,
               "PUT",
               host,
               path
           );
}


bool
us_http_response_header_set_content_length (
    us_http_response_header_t *self,
    int32_t content_length
)
{
    char content_length_str[32];
    US_DEFAULT_SNPRINTF ( content_length_str, sizeof ( content_length_str ), "%ld", ( long ) content_length );
    return self->m_items->add ( self->m_items, "Content-Length", content_length_str ) != 0;
}


bool
us_http_response_header_set_content_type (
    us_http_response_header_t *self,
    const char *content_mime_type
)
{
    return self->m_items->add ( self->m_items, "Content-Type", content_mime_type ) != 0;
}


int32_t
us_http_response_header_get_content_length (
    const us_http_response_header_t *self
)
{
    /* TODO */
    return 0;
}

const char *
us_http_response_header_get_mime_type (
    const us_http_response_header_t *self
)
{
    /* TODO */
    return 0;
}



us_http_response_header_t *
us_http_response_header_create_error (
    us_allocator_t *allocator,
    int32_t http_error_code,
    const char *content_type,
    uint32_t content_length
)
{
    us_http_response_header_t *self;
    self = us_http_response_header_create ( allocator );
    if ( self )
    {
        self->m_code = http_error_code;
        if ( !us_http_response_header_set_content_type ( self, content_type ) ||
                !us_http_response_header_set_content_length ( self, content_length ) )
        {
            self->destroy ( self );
            self = 0;
        }
    }
    return self;
}

us_http_response_header_t *
us_http_response_header_create_redirect (
    us_allocator_t *allocator,
    int32_t http_redirect_code,
    const char *redirect_to_url
)
{
    us_http_response_header_t *self;
    self = us_http_response_header_create ( allocator );
    if ( self )
    {
        self->m_code = http_redirect_code;
        if ( !self->m_items->add ( self->m_items, "Location", redirect_to_url ) )
        {
            self->destroy ( self );
            self = 0;
        }
    }
    return self;
}

us_http_response_header_t *
us_http_response_header_create_ok (
    us_allocator_t *allocator,
    int32_t http_ok_code,
    const char *content_type,
    uint32_t content_length
)
{
    us_http_response_header_t *self;
    self = us_http_response_header_create ( allocator );
    if ( self )
    {
        self->m_code = http_ok_code;
        if ( !us_http_response_header_set_content_type ( self, content_type ) ||
                !us_http_response_header_set_content_length ( self, content_length ) )
        {
            self->destroy ( self );
            self = 0;
        }
    }
    return self;
}


bool
us_http_response_header_flatten (
    us_http_response_header_t *self,
    us_buffer_t *buf
)
{
    bool r = true;
    r &= us_buffer_append_string ( buf, self->m_version );
    r &= us_buffer_append_string ( buf, " " );
    {
        char status_code_str[16];
        US_DEFAULT_SNPRINTF ( status_code_str, 16, "%03d", ( self->m_code % 1000 ) );
        r &= us_buffer_append_string ( buf, status_code_str );
    }
    r &= us_buffer_append_string ( buf, " " );
    r &= us_buffer_append_string ( buf, us_http_reason_phrase ( self->m_code ) );
    r &= us_buffer_append_string ( buf, "\r\n" );
    r &= us_http_header_item_list_flatten ( self->m_items, buf );
    return r;
}

bool
us_http_request_header_flatten (
    us_http_request_header_t *self,
    us_buffer_t *buf
)
{
    bool r = false;
    if ( self->m_method && self->m_path && self->m_version )
    {
        r = true;
        r &= us_buffer_append_string ( buf, self->m_method );
        r &= us_buffer_append_string ( buf, " " );
        r &= us_buffer_append_string ( buf, self->m_path );
        r &= us_buffer_append_string ( buf, " " );
        r &= us_buffer_append_string ( buf, self->m_version );
        r &= us_buffer_append_string ( buf, "\r\n" );
        r &= us_http_header_item_list_flatten ( self->m_items, buf );
    }
    return r;
}

bool
us_http_header_item_list_flatten (
    us_http_header_item_list_t *self,
    us_buffer_t *buf
)
{
    bool r = true;
    us_http_header_item_t *item = self->m_first;
    while ( item )
    {
        r &= us_buffer_append_string ( buf, item->m_key );
        r &= us_buffer_append_string ( buf, ": " );
        r &= us_buffer_append_string ( buf, item->m_value );
        r &= us_buffer_append_string ( buf, "\r\n" );
        item = item->m_next;
    }
    r &= us_buffer_append_string ( buf, "\r\n" );
    return r;
}

us_http_response_header_t *
us_http_response_header_parse (
    us_allocator_t *allocator,
    us_buffer_t *buf
)
{
    us_http_response_header_t *r = 0;
    us_http_response_header_t *self;
    self = us_http_response_header_create ( allocator );
    if ( self )
    {
        int32_t len;
        len = us_buffer_find_string_len ( buf, ' ', '\n' );
        if ( len != -1 )
        {
            us_http_response_header_set_versionn (
                self,
                us_buffer_read_ptr ( buf ),
                len
            );
            if ( us_buffer_advance ( buf, len + 1 ) )
            {
                len = us_buffer_find_string_len ( buf, ' ', '\n' );
                if ( len == 3 )
                {
                    int status_code = 0;
                    const char * p = us_buffer_read_ptr ( buf );
                    status_code += ( p[0] - '0' ) * 100;
                    status_code += ( p[1] - '0' ) * 10;
                    status_code += ( p[2] - '0' ) * 1;
                    self->m_code = status_code;
                    if ( us_buffer_skip_to_delim ( buf, "\n" ) )
                    {
                        if ( us_buffer_advance ( buf, 1 ) )
                        {
                            if ( us_http_header_item_list_parse ( self->m_items, buf ) )
                            {
                                r = self;
                            }
                        }
                    }
                }
            }
        }
    }
    if ( r == 0 && self != 0 )
    {
        self->destroy ( self );
    }
    return r;
}

us_http_request_header_t *
us_http_request_header_parse (
    us_allocator_t *allocator,
    us_buffer_t *buf
)
{
    us_http_request_header_t *r = 0;
    us_http_request_header_t *self;
    self = us_http_request_header_create ( allocator );
    if ( self )
    {
        int32_t len;
        len = us_buffer_find_string_len ( buf, ' ', '\n' );
        if ( len != -1 )
        {
            us_http_request_header_set_methodn (
                self,
                us_buffer_read_ptr ( buf ),
                len
            );
            if ( us_buffer_advance ( buf, len + 1 ) )
            {
                len = us_buffer_find_string_len ( buf, ' ', '\n' );
                if ( len > 0 )
                {
                    us_http_request_header_set_pathn (
                        self,
                        us_buffer_read_ptr ( buf ),
                        len
                    );
                    if ( us_buffer_advance ( buf, len + 1 ) )
                    {
                        len = us_buffer_find_string_len ( buf, '\r', '\n' );
                        if ( len > 0 )
                        {
                            us_http_request_header_set_versionn (
                                self,
                                us_buffer_read_ptr ( buf ),
                                len
                            );
                            if ( self->m_method && self->m_path && self->m_version )
                            {
                                if ( us_buffer_advance ( buf, len + 1 ) )
                                {
                                    if ( us_http_header_item_list_parse ( self->m_items, buf ) )
                                    {
                                        r = self;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if ( r == 0 && self != 0 )
    {
        self->destroy ( self );
    }
    return r;
}

bool
us_http_header_item_list_parse (
    us_http_header_item_list_t *self,
    us_buffer_t *buf
)
{
    bool r = true;
    bool done = false;
    while ( !done )
    {
        const char *p = us_buffer_read_ptr ( buf );
        if ( *p == '\r' || *p == '\n' )
        {
            us_buffer_advance ( buf, 1 );
            p = us_buffer_read_ptr ( buf );
            if ( *p == '\n' )
            {
                p = us_buffer_read_ptr ( buf );
                us_buffer_advance ( buf, 1 );
            }
            done = true;
        }
        else
        {
            const char *key;
            int key_len;
            const char *value;
            int value_len;
            key = p;
            key_len = us_buffer_find_string_len ( buf, ':', '\n' );
            us_buffer_advance ( buf, key_len + 1 );
            us_buffer_skip_delim ( buf, " \t" );
            value = us_buffer_read_ptr ( buf );
            value_len = us_buffer_find_string_len ( buf, '\r', '\n' );
            us_buffer_advance ( buf, value_len + 1 );
            if ( key && key_len > 0 && value && value_len > 0 )
            {
                self->addn ( self, key, key_len, value, value_len );
            }
            else
            {
                r = false;
                done = false;
            }
        }
    }
    return r;
}

const char *
us_http_reason_phrase ( int code )
{
    const char *r;
    switch ( code )
    {
    case 100:
        r = "Continue";
        break;
    case 101:
        r = "Switching Protocols";
        break;
    case 200:
        r = "OK";
        break;
    case 201:
        r = "Created";
        break;
    case 202:
        r = "Accepted";
        break;
    case 203:
        r = "Non-Authoritative Information";
        break;
    case 204:
        r = "No Content";
        break;
    case 205:
        r = "Reset Content";
        break;
    case 206:
        r = "Partial Content";
        break;
    case 300:
        r = "Multiple Choices";
        break;
    case 301:
        r = "Moved Permanently";
        break;
    case 302:
        r = "Found";
        break;
    case 303:
        r = "See Other";
        break;
    case 304:
        r = "Not Modified";
        break;
    case 305:
        r = "Use Proxy";
        break;
    case 307:
        r = "Temporary Redirect";
        break;
    case 400:
        r = "Bad Request";
        break;
    case 401:
        r = "Unauthorized";
        break;
    case 402:
        r = "Payment Required";
        break;
    case 403:
        r = "Forbidden";
        break;
    case 404:
        r = "Not Found";
        break;
    case 405:
        r = "Method Not Allowed";
        break;
    case 406:
        r = "Not Acceptable";
        break;
    case 407:
        r = "Proxy Authentication Required";
        break;
    case 408:
        r = "Request Time-out";
        break;
    case 409:
        r = "Conflict";
        break;
    case 410:
        r = "Gone";
        break;
    case 411:
        r = "Length Required";
        break;
    case 412:
        r = "Precondition Failed";
        break;
    case 413:
        r = "Request Entity Too Large";
        break;
    case 414:
        r = "Request-URI Too Large";
        break;
    case 415:
        r = "Unsupported Media Type";
        break;
    case 416:
        r = "Requested range not satisfiable";
        break;
    case 417:
        r = "Expectation Failed";
        break;
    case 500:
        r = "Internal Server Error";
        break;
    case 501:
        r = "Not Implemented";
        break;
    case 502:
        r = "Bad Gateway";
        break;
    case 503:
        r = "Service Unavailable";
        break;
    case 504:
        r = "Gateway Time-out";
        break;
    case 505:
        r = "HTTP Version not supported";
        break;
    default:
        r = "Unknown";
        break;
    }
    return r;
}

