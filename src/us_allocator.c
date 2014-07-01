#include "us_world.h"

#include "us_allocator.h"

#include "us_print.h"
#include "us_logger.h"

/*
  Copyright (c) 2013, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of J.D. Koftinoff Software, Ltd. nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD. BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

char *us_strdup( struct us_allocator_s *allocator, const char *src )
{
    size_t len = strlen( src );
    char *p = us_new_array( allocator, char, len + 1 );
    if ( p )
    {
        strcpy( p, src );
    }
    return p;
}

char *us_strndup( struct us_allocator_s *allocator, const char *src, int chars_to_copy )
{
    int len = chars_to_copy;
    char *p = us_new_array( allocator, char, len + 1 );
    if ( p )
    {
        memcpy( p, src, len );
        p[len] = '\0';
    }
    return p;
}

us_allocator_t *us_simple_allocator_init( us_simple_allocator_t *self, void *raw_memory, int32_t raw_memory_length )
{
    self->m_raw_memory = raw_memory;
    self->m_raw_memory_length = raw_memory_length;
    self->m_current_position = 0;
    self->m_base.destroy = us_simple_allocator_destroy;
    self->m_base.alloc = us_simple_allocator_alloc;
    self->m_base.realloc = us_simple_allocator_realloc;
    self->m_base.free = us_simple_allocator_free;
    return &self->m_base;
}

void us_simple_allocator_destroy( us_allocator_t *self )
{
    /* Do Nothing */
    (void)self;
}

void *us_simple_allocator_alloc( us_allocator_t *self_, int32_t length, int32_t count )
{
    us_simple_allocator_t *self = (us_simple_allocator_t *)self_;
    void *result = 0;
    /* round up total size to 32 bit alignment */
    int32_t total_size_to_allocate = us_round_size( length * count );
    if ( ( self->m_current_position + total_size_to_allocate ) <= ( self->m_raw_memory_length ) )
    {
        result = ( (char *)self->m_raw_memory ) + self->m_current_position;
        self->m_current_position += total_size_to_allocate;
    }
    return result;
}

void *us_simple_allocator_realloc( struct us_allocator_s *self_, const void *orig_ptr, int32_t length, int32_t count )
{
    void *new_ptr = self_->alloc( self_, length, count );
    if ( new_ptr )
    {
        memcpy( new_ptr, orig_ptr, us_round_size( length * count ) );
        self_->free( self_, orig_ptr );
    }
    return new_ptr;
}

void us_simple_allocator_free( struct us_allocator_s *self_, const void *ptr )
{
    (void)self_;
    (void)ptr;
    /* Do nothing; the simple allocator never frees */
}

void us_simple_allocator_reset( us_simple_allocator_t *self )
{
    self->m_current_position = 0;
}

#if US_ENABLE_MALLOC

us_allocator_t *us_malloc_allocator_init( us_malloc_allocator_t *self )
{
    self->m_base.destroy = us_malloc_allocator_destroy;
    self->m_base.alloc = us_malloc_allocator_alloc;
    self->m_base.realloc = us_malloc_allocator_realloc;
    self->m_base.free = us_malloc_allocator_free;
    return &self->m_base;
}

void us_malloc_allocator_destroy( us_allocator_t *self_ )
{
    (void)self_;
    /* nothing to destroy, we do not keep track of all mallocs and frees */
}

void *us_malloc_allocator_alloc( us_allocator_t *self, int32_t length, int32_t count )
{
    (void)self;
    return US_DEFAULT_MALLOC( us_round_size( length * count ) );
}

void *us_malloc_allocator_realloc( struct us_allocator_s *self, const void *orig_ptr, int32_t length, int32_t count )
{
    (void)self;
    return US_DEFAULT_REALLOC( (void *)orig_ptr, us_round_size( length * count ) );
}

void us_malloc_allocator_free( struct us_allocator_s *self, const void *ptr )
{
    (void)self;
    US_DEFAULT_FREE( (void *)ptr );
}

#endif
