#ifndef US_JSON_H
#define US_JSON_H


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
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_json_parser.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct us_json_s;

typedef struct us_json_entry_s
{
    us_allocator_t *m_allocator;
    const char *m_key;

    union
    {
        struct us_json_s *m_value_json;
        const char *m_value_string_ptr;
        int32_t *m_value_int32_ptr;
    };

    enum 
    {
        us_json_type_none,
        us_json_type_json,
        us_json_type_string_ptr,
        us_json_type_int32_ptr
    } m_type;

    struct us_json_entry_s *m_next;
} us_json_entry_t;

us_json_entry_t *us_json_entry_create( us_allocator_t *allocator, const char *key );
void us_json_entry_destroy( us_json_entry_t *self );

void us_json_entry_remove_value( us_json_entry_t *self );
void us_json_entry_set_value_json( us_json_entry_t *self, struct us_json_s *value );
void us_json_entry_set_value_string_ptr( us_json_entry_t *self, const char*s );
void us_json_entry_set_value_int32_ptr( us_json_entry_t *self, int32_t *value );

bool us_json_entry_flatten_to_buffer( const us_json_entry_t *self, us_buffer_t *buffer );

typedef struct us_json_s
{
    void (*destroy)( struct us_json_s *self );
    us_allocator_t *m_allocator;
    us_json_entry_t *m_first_item;
    us_json_entry_t *m_last_item;
} us_json_t; 

us_json_t *us_json_create( us_allocator_t *allocator );
void us_json_destroy( us_json_t *self );

us_json_entry_t *us_json_append_entry( us_json_t *self, us_json_entry_t *entry );
us_json_entry_t *us_json_append_string_ptr( us_json_t *self, const char *key, const char *value );
us_json_entry_t *us_json_append_int32_ptr( us_json_t *self, const char *key, int32_t *value );
us_json_t *us_json_append_object( us_json_t *self, const char *key );

bool us_json_flatten_to_buffer( const us_json_t *self, us_buffer_t *buffer );

#ifdef __cplusplus
}
#endif


#endif
