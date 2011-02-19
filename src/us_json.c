
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
#include "us_json.h"


us_json_entry_t *us_json_entry_create( us_allocator_t *allocator, const char *key )
{
    us_json_entry_t *self = us_new( allocator, us_json_entry_t );
    if( self )
    {
        self->m_allocator = allocator;
        self->m_key=key;
        self->m_type=us_json_type_none;
        self->m_next = 0;
    }
    return self;
}

void us_json_entry_destroy( us_json_entry_t *self )
{
    if( self->m_next )
    {
        us_json_entry_destroy( self->m_next );
    }
    us_json_entry_remove_value( self );
    us_delete( self->m_allocator, self );
}

void us_json_entry_remove_value( us_json_entry_t *self )
{
    if( self->m_type == us_json_type_json )
    {
        us_json_destroy( self->value.m_value_json );
    }
    else if( self->m_type == us_json_type_string_buffer )
    {
        self->value.m_value_string_buffer->destroy( self->value.m_value_string_buffer );
    }
    self->m_type = us_json_type_none;
}

void us_json_entry_set_value_json( us_json_entry_t *self, struct us_json_s *value )
{
    us_json_entry_remove_value( self );
    self->value.m_value_json = value;
    self->m_type = us_json_type_json;
}

void us_json_entry_set_value_string_ptr( us_json_entry_t *self, const char *s )
{
    us_json_entry_remove_value( self );
    self->value.m_value_string_ptr = s;
    self->m_type = us_json_type_string_ptr;
}

void us_json_entry_set_value_int32_ptr( us_json_entry_t *self, int32_t *value )
{
    us_json_entry_remove_value( self );
    self->value.m_value_int32_ptr = value;
    self->m_type = us_json_type_int32_ptr;
}

void us_json_entry_set_value_string_buffer( us_json_entry_t *self, us_buffer_t *buf )
{
    us_json_entry_remove_value( self );
    self->value.m_value_string_buffer = buf;
    self->m_type = us_json_type_string_buffer;
}

void us_json_entry_set_value_int32( us_json_entry_t *self, int32_t value )
{
    us_json_entry_remove_value( self );
    self->value.m_value_int32 = value;
    self->m_type = us_json_type_int32;
}


bool us_json_entry_flatten_to_buffer( const us_json_entry_t *self, us_buffer_t *buffer )
{
    bool r=true;
    if( self->m_key )
    {
        r&=us_buffer_append_string( buffer, "\"" );
        /* TODO: Do json escaping */
        r&=us_buffer_append_string( buffer, self->m_key );
        r&=us_buffer_append_string( buffer, "\" : " );
    }
    switch( self->m_type )
    {
    case us_json_type_none:
        r&=us_buffer_append_string( buffer, "null" );
        break;
    case us_json_type_json:
        r&=us_json_flatten_to_buffer( self->value.m_value_json, buffer );
        break;
    case us_json_type_string_ptr:
        r&=us_buffer_append_string( buffer, "\"" );
        /* TODO: Do json escaping */
        r&=us_buffer_append_string( buffer, self->value.m_value_string_ptr );
        r&=us_buffer_append_string( buffer, "\"" );
        break;
    case us_json_type_int32_ptr:
    {
        char s[32];
        US_DEFAULT_SNPRINTF( s, sizeof(s), "%d", *self->value.m_value_int32_ptr );
        r&=us_buffer_append_string( buffer, s );
    }
    case us_json_type_int32:
    {
        char s[32];
        US_DEFAULT_SNPRINTF( s, sizeof(s), "%d", self->value.m_value_int32 );
        r&=us_buffer_append_string( buffer, s );
    }
    break;
    case us_json_type_string_buffer:
        r&=us_buffer_append_string( buffer, "\"" );
        /* TODO: Do json escaping */
        r&=us_buffer_write_buffer( buffer, self->value.m_value_string_buffer );
        r&=us_buffer_append_string( buffer, "\"" );
        break;
    default:
        us_log_error( "invalid json entry type" );
        r=false;
        break;
    }
    return r;
}


us_json_t *us_json_create( us_allocator_t *allocator )
{
    us_json_t *self = us_new( allocator, us_json_t );
    if( self )
    {
        self->m_allocator = allocator;
        self->m_is_array = false;
        self->destroy = us_json_destroy;
        self->m_first_item=0;
        self->m_last_item=0;
    }
    return self;
}

void us_json_destroy( us_json_t *self )
{
    if( self )
    {
        us_json_entry_t *cur = self->m_first_item;
        if( cur )
        {
            us_json_entry_destroy( cur );
        }
        self->m_first_item = 0;
        self->m_last_item = 0;
    }
}


us_json_entry_t *us_json_append_entry( us_json_t *self, us_json_entry_t *entry )
{
    if( self && entry )
    {
        if( self->m_last_item )
        {
            self->m_last_item->m_next = entry;
            self->m_last_item = entry;
        }
        else
        {
            self->m_first_item=entry;
            self->m_last_item=entry;
        }
    }
    if( !self && entry )
    {
        us_json_entry_destroy( entry );
        entry=0;
    }
    return entry;
}

us_json_entry_t *us_json_append_string_ptr( us_json_t *self, const char *key, const char *value )
{
    us_json_entry_t *entry = us_json_entry_create( self->m_allocator, key );
    if( entry )
    {
        us_json_entry_set_value_string_ptr( entry, value );
    }
    return us_json_append_entry( self, entry );
}

us_json_entry_t *us_json_append_int32_ptr( us_json_t *self, const char *key, int32_t *value )
{
    us_json_entry_t *entry = us_json_entry_create( self->m_allocator, key );
    if( entry )
    {
        us_json_entry_set_value_int32_ptr( entry, value );
    }
    return us_json_append_entry( self, entry );
}


us_json_t *us_json_append_object( us_json_t *self, const char *key )
{
    us_json_t *j=0;
    us_json_entry_t *entry = us_json_entry_create( self->m_allocator, key );
    if( entry )
    {
        j = us_json_create( self->m_allocator );
        if( j )
        {
            us_json_entry_set_value_json( entry, j );
        }
    }
    if( !us_json_append_entry( self, entry ) )
    {
        j=0;
    }
    return j;
}

bool us_json_flatten_to_buffer( const us_json_t *self, us_buffer_t *buffer )
{
    bool r=true;
    us_json_entry_t *entry=self->m_first_item;
    us_buffer_append_string( buffer, self->m_is_array ? "[ " : "{ " );
    while( entry )
    {
        us_json_entry_flatten_to_buffer( entry, buffer );
        if( entry->m_next )
        {
            us_buffer_append_string( buffer, ",\n" );
        }
        entry=entry->m_next;
    }
    us_buffer_append_string( buffer, self->m_is_array ? " ]\n" : " }\n" );
    return r;
}


