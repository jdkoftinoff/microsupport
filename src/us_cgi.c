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
#include "us_cgi.h"


bool us_cgi_is_escapable( char c )
{
  bool r=true;
  
  if( isalnum(c) )
  {
    r=false;
  }
  else
  {
    if( strchr( ";/?:@&=+!*'(),$-_.%<>~", c )!=0 )
      r=false;
  }
  return r;
}

bool us_cgi_is_escapable_char_with_amp( char c )
{
  bool r=true;
  
  if( isalnum(c) )
  {
    r=false;
  }
  else
  {
    if( strchr( ";/?:@=+!*'(),$-_.%<>~", c )!=0 )
      r=false;
  }
  
  return r;
}


bool us_cgi_is_escapable_char_for_var( char c )
{
  bool r=true;
  
  if( isalnum(c) )
  {
    r=false;
  }
  else
  {
    if( strchr( ";/?:@&=+!*'(),$-_.%<>~", c )!=0 )
      r=true;
  }
  
  return r;
}

bool us_cgi_is_escapable_char_with_amp_for_var( char c )
{
  bool r=true;
  
  if( isalnum(c) )
  {
    r=false;
  }
  else
  {
    if( strchr( ";/?:@=+!*'(),$-_.%<>~", c )!=0 )
      r=true;
  }
  
  return r;
}


bool us_cgi_unescape( const char *src, int src_len, us_buffer_t *dest_buf )
{
  /* TODO: */
  return false;
}

bool us_cgi_escape( const char *src, int src_len, us_buffer_t *dest_buf )
{
  /* TODO: */
  return false;
}

bool us_cgi_unescape_with_amp( const char *src, int src_len, us_buffer_t *dest_buf )
{
  /* TODO: */
  return false;
}

bool us_cgi_escape_with_amp( const char *src, int src_len, us_buffer_t *dest_buf )
{
  /* TODO: */
  return false;
}

bool us_cgi_extract_pair( const char *src, int src_len, char *key, int max_key_len, char *value, int max_value_len )
{
  /* TODO: */
  return false;
}

bool us_cgi_encode_pair( const char *key, const char *value, us_buffer_t *dest_buf )
{
  /* TODO: */
  return false;
}
