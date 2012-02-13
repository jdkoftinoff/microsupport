#ifndef US_ALLOCATOR_H
#define US_ALLOCATOR_H

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

#ifdef __cplusplus
extern "C"
{
#endif

    /**
       \addtogroup us_allocator Memory Allocator
    */
    /*@{*/


    /** us_new

        A define which simulates c++ new() operator, except it accepts an
        allocator to allocate from.
        No initializers of the objects are called.
    */

#define us_new( ALLOCATOR, T )                                      \
    (T *)((ALLOCATOR)->alloc( (ALLOCATOR), (int32_t)sizeof( T ), 1 ))

#define us_delete( ALLOCATOR, PTR )             \
    do { if( (ALLOCATOR)!=0 && (PTR)!=0 ) (ALLOCATOR)->free( (ALLOCATOR), (PTR)); } while(false)


    /** us_new_array

        A define which simulates c++ new [] operator, except it accepts an
        allocator to allocate from.
    */

#define us_new_array( ALLOCATOR, T, COUNT )                             \
    (T *)((ALLOCATOR)->alloc( (ALLOCATOR), (int32_t)sizeof( T ), (int32_t)(COUNT) ))

    /** us_round_size

        Calculate size in bytes rounded up to the nearest 32bit word size.
    */
#define us_round_size( VALUE )                                          \
    (((VALUE) + (uint32_t)(sizeof (uint32_t))-1) & (uint32_t)~(sizeof (uint32_t)-1))


    /** us_allocator_t

        The us_allocator implements a
        simple growing stack of allocations within a fixed buffer size
    */
    typedef struct us_allocator_s
    {
        /**
           Destroy the allocator.
        */

        void ( *destroy ) (
            struct us_allocator_s *self
        );

        /**
           Allocate memory from the allocator.
        */
        void * ( *alloc ) (
            struct us_allocator_s *self,
            int32_t length,
            int32_t count
        );


        /**
           reallocate memory from the allocator.
        */
        void * ( *realloc ) (
            struct us_allocator_s *self,
            const void *orig_ptr,
            int32_t length,
            int32_t count
        );

        void ( *free ) (
            struct us_allocator_s *self,
            const void *ptr
        );
    } us_allocator_t;

    typedef struct us_simple_allocator_s
    {
        us_allocator_t m_base;
        /** pointer to the raw memory pool to allocate from */
        void *m_raw_memory;

        /** length of the raw memory pool */
        int32_t m_raw_memory_length;

        /** the current position in the raw memory pool */
        int32_t m_current_position;
    } us_simple_allocator_t;

    /**
       us_simple_allocator_init
       Initialize an us_allocator_t object, attaching it to a block of memory
       with a specific size in bytes

       @param self object to initialize
       @param raw_memory pointer to raw memory
       @param raw_memory_length size of raw memory in bytes
       @returns pointer initialized object
    */

    us_allocator_t *
    us_simple_allocator_init (
        us_simple_allocator_t *self,
        void *raw_memory,
        int32_t raw_memory_length
    );

    void
    us_simple_allocator_reset (
        us_simple_allocator_t *self
    );

    /** us_simple_allocator_destroy
        simple allocators are allocated from raw memory and have no need to
        do anything to destroy the pool of memory
    */

    void us_simple_allocator_destroy (
        struct us_allocator_s *self
    );

    /** us_simple_allocator_alloc

        allocate some memory from the allocator's memory pool.

        @param self allocator to allocate from
        @param length in bytes of object to allocate
        @param count count of objects to allocate
        @returns memory void * or NULL if error
    */
    void *us_simple_allocator_alloc (
        struct us_allocator_s *self,
        int32_t length,
        int32_t count
    );

    void *us_simple_allocator_realloc (
        struct us_allocator_s *self,
        const void *orig_ptr,
        int32_t length,
        int32_t count
    );

    void us_simple_allocator_free (
        struct us_allocator_s *self,
        const void *ptr
    );

#if US_ENABLE_MALLOC

    typedef struct us_malloc_allocator_s
    {
        us_allocator_t m_base;
    } us_malloc_allocator_t;

    /**
       us_malloc_allocator_init
       simple wrapper of malloc/free

       @param self object to initialize
       @returns pointer initialized object
    */

    us_allocator_t *
    us_malloc_allocator_init (
        struct us_malloc_allocator_s *self
    );

    /** us_malloc_allocator_destroy
        simple allocators are allocated from raw memory and have no need to
        do anything to destroy the pool of memory
    */

    void us_malloc_allocator_destroy (
        struct us_allocator_s *self
    );


    void *us_malloc_allocator_alloc (
        struct us_allocator_s *self,
        int32_t length,
        int32_t count
    );

    void *us_malloc_allocator_realloc (
        struct us_allocator_s *self,
        const void *orig_ptr,
        int32_t length,
        int32_t count
    );

    void us_malloc_allocator_free (
        struct us_allocator_s *self,
        const void *ptr
    );


#endif

    char * us_strdup ( struct us_allocator_s *allocator, const char *src );

    char * us_strndup ( struct us_allocator_s *allocator, const char *src, int chars_to_copy );

    /*@}*/

#ifdef __cplusplus
}
#endif

#endif
