
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
 * Neither the name of the <organization> nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
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
us_http_header_item_list_create( us_allocator_t *allocator )
{
  us_http_header_item_list_t *self = us_new( allocator, us_http_header_item_list_t );
  if( self )
  {
    self->add = us_http_header_item_list_add;
    self->destroy = us_http_header_item_list_destroy;
    self->find = us_http_header_item_list_find;
    self->remove = us_http_header_item_list_remove;
    self->m_allocator = allocator;
    self->m_first = 0;
  }
  
  return self;
}


void us_http_header_item_list_destroy( us_http_header_item_list_t *self )
{
  us_delete( self->m_allocator, self );
}

us_http_header_item_t *
us_http_header_item_list_add(
                             us_http_header_item_list_t *self,
                             const char *key,
                             const char *value
                             )
{
  us_http_header_item_t *item;
  item = us_new( self->m_allocator, us_http_header_item_t );
  if( item )
  {
    item->m_key = 0;
    item->m_value = 0;
    
    item->m_key = us_strdup(self->m_allocator, key );
    if( item->m_key )
    {          
      item->m_value =us_strdup(self->m_allocator, value );
    }

    if( item->m_key==0 || item->m_value==0 )
    {
      us_delete( self->m_allocator, item->m_key );
      us_delete( self->m_allocator, item->m_value );
      us_delete( self->m_allocator, item );
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
us_http_header_item_list_remove(
                                us_http_header_item_list_t *self,
                                us_http_header_item_t *item
                                )
{
  bool r=false;
  us_http_header_item_t *prev = 0;
  us_http_header_item_t *i = self->m_first;
  while (i)
  {
    if( i==item )
    {      
      prev->m_next = i->m_next;
      us_delete( self->m_allocator, i->m_key );
      us_delete( self->m_allocator, i->m_value );
      us_delete( self->m_allocator, i );
      r=true;
      break;
    }
    
    prev = i;
    i = i->m_next;
  };
  return r;
}


us_http_header_item_t *
us_http_header_item_list_find(
                              us_http_header_item_list_t *self,
                              const char *key
                              )
{
  us_http_header_item_t *r = 0;
  us_http_header_item_t *i = self->m_first;
  while (i)
  {
    if( strncasecmp( i->m_key, key, strlen(key) )==0 )
    {
      r= i;
      break;
    }
    i = i->m_next;
  };
  return r;
}




us_http_request_header_t *
us_http_request_header_create( us_allocator_t *allocator )
{
  us_http_request_header_t *self;
  
  self=us_new( allocator, us_http_request_header_t );
  if( self )
  {
    self->m_allocator = allocator;
    self->destroy = us_http_request_header_destroy;
    self->m_items = us_http_header_item_list_create(allocator);
    self->m_method = 0;
    self->m_path = 0;
    self->m_version = "HTTP/1.1";
    if( self->m_items==0 )
    {
      us_delete( allocator, self );
      self = 0;
    }
  }
  return self;
}



void us_http_request_header_destroy( us_http_request_header_t *self )
{
  self->m_items->destroy( self->m_items );
  us_delete( self->m_allocator, self->m_path );
  us_delete( self->m_allocator, self );  
}



us_http_response_header_t *
us_http_response_header_create( us_allocator_t *allocator )
{
  us_http_response_header_t *self;
  self = us_new( allocator, us_http_response_header_t );
  if( self )
  {
    self->m_allocator = allocator;
    self->destroy = us_http_response_header_destroy;
    self->m_code = 0;
    self->m_items = us_http_header_item_list_create(allocator);
    if( !self->m_items )
    {
      us_delete( allocator, self );
      self = 0;
    }
  }
  return self;  
}



void us_http_response_header_destroy( us_http_response_header_t *self )
{
  self->m_items->destroy( self->m_items );
  us_delete( self->m_allocator, self );
}

us_http_request_header_t *
us_http_request_header_create_helper(
                                     us_allocator_t *allocator,
                                     const char *method,
                                     const char *host,
                                     const char *path
                                     )
{
  us_http_request_header_t *self;
  self = us_http_request_header_create(allocator);
  if( self )
  {
    self->m_method = method;
    self->m_path = us_strdup(allocator, path);
    if( self->m_path==0 )
    {
      self->destroy( self );
      self = 0;
    }
    else 
    {
      if( !self->m_items->add( self->m_items, "Host", host ) )
      {
        self->destroy(self);
        self = 0;
      }
    }

  }
  return self;
}


us_http_request_header_t *
us_http_request_header_create_get(
                                  us_allocator_t *allocator,
                                  const char *host,
                                  const char *path
                                  )
{
  return us_http_request_header_create_helper(
                                              allocator,
                                              "GET", 
                                              host,
                                              path 
                                              );
}

us_http_request_header_t *
us_http_request_header_create_delete(
                                     us_allocator_t *allocator,
                                     const char *host,
                                     const char *path
                                     )
{
  return us_http_request_header_create_helper(
                                              allocator,
                                              "DELETE", 
                                              host,
                                              path 
                                              );
}


us_http_request_header_t *
us_http_request_header_create_post(
                                   us_allocator_t *allocator,
                                   const char *host,
                                   const char *path,
                                   const char *content_type,
                                   uint32_t content_length
                                   )
{
  return us_http_request_header_create_helper(
                                              allocator,                                              
                                              "POST", 
                                              host,
                                              path 
                                              );
}


us_http_request_header_t *
us_http_request_header_create_put(
                                  us_allocator_t *allocator,
                                  const char *host,
                                  const char *path,
                                  const char *content_type,
                                  uint32_t content_length
                                  )
{
  return us_http_request_header_create_helper(
                                              allocator,                                              
                                              "PUT", 
                                              host,
                                              path 
                                              );
}


bool
us_http_response_header_set_content_length(
                                           us_http_response_header_t *self,
                                           int32_t content_length
                                           )
{
  char content_length_str[32];
  US_DEFAULT_SNPRINTF( content_length_str, sizeof( content_length_str ), "%ld", (long)content_length );
  return self->m_items->add( self->m_items, "Content-Length", content_length_str ) != 0;
}


bool
us_http_response_header_set_content_type(
                                         us_http_response_header_t *self,
                                         const char *content_mime_type
                                         )
{
  return self->m_items->add( self->m_items, "Content-Type", content_mime_type ) != 0;  
}


int32_t
us_http_response_header_get_content_length(
                                           const us_http_response_header_t *self
                                           )
{
  /* TODO */
  return 0;
}

const char *
us_http_response_header_get_mime_type(
                                      const us_http_response_header_t *self
                                      )
{
  /* TODO */
  return 0;
}



us_http_response_header_t *
us_http_response_header_create_error(
                                     us_allocator_t *allocator,
                                     int32_t http_error_code,
                                     const char *content_type,
                                     uint32_t content_length
                                     )
{
  us_http_response_header_t *self;

  self = us_http_response_header_create(allocator);
  if( self )
  {
    self->m_code = http_error_code;
    if( !us_http_response_header_set_content_type(self,content_type) ||
        !us_http_response_header_set_content_length(self,content_length) )
    {
      self->destroy( self );
      self = 0;
    }
  }  
  return self;
}

us_http_response_header_t *
us_http_response_header_create_redirect(
                                        us_allocator_t *allocator,
                                        int32_t http_redirect_code,
                                        const char *redirect_to_url
                                        )
{
  us_http_response_header_t *self;
  
  self = us_http_response_header_create(allocator);
  if( self )
  {
    self->m_code = http_redirect_code;
    if( !self->m_items->add( self->m_items, "Location", redirect_to_url ) )
    {
      self->destroy( self );
      self = 0;
    }
  }  
  return self;
}

us_http_response_header_t *
us_http_response_header_create_ok(
                                  us_allocator_t *allocator,
                                  int32_t http_ok_code,
                                  const char *content_type,
                                  uint32_t content_length
                                  )
{
  us_http_response_header_t *self;
  
  self = us_http_response_header_create(allocator);
  if( self )
  {
    self->m_code = http_ok_code;
    if( !us_http_response_header_set_content_type(self,content_type) ||
       !us_http_response_header_set_content_length(self,content_length) )
    {
      self->destroy( self );
      self = 0;
    }
  }  
  return self;
}


bool
us_http_response_header_flatten(
                                us_http_response_header_t *self,
                                us_buffer_t *buf
                                )
{
  /* TODO */
  return false;
}

bool
us_http_request_header_flatten(
                               us_http_request_header_t *self,
                               us_buffer_t *buf
                               )
{
  /* TODO */
  return false;
}

bool
us_http_header_item_list_flatten(
                                 us_http_header_item_list_t *self,
                                 us_buffer_t *buf
                                 )
{
  /* TODO */
  return false;
}

bool
us_http_response_header_parse(
                              us_http_response_header_t *self,
                              us_buffer_t *buf
                              )
{
  /* TODO */
  return false;
}

bool
us_http_request_header_parse(
                             us_http_request_header_t *self,
                             us_buffer_t *buf
                             )
{
  /* TODO */
  return false;
}

bool
us_http_header_item_list_parse(
                               us_http_header_item_list_t *self,
                               us_buffer_t *buf
                               )
{
  /* TODO */
  return false;
}


