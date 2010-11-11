#ifndef US_ALLOCATOR_HEAP_H
#define US_ALLOCATOR_HEAP_H

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

/**
   \addtogroup us_allocator Memory Allocator
*/
/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif


    /**< Doubly linked list for a single free or allocated block */
    typedef struct us_allocator_heap_block_s
    {
        /**< next block in line, or null */
        struct us_allocator_heap_block_s *m_next;
        /**< previous block in line, or null */
        struct us_allocator_heap_block_s *m_prev;
        /**< size of this block payload in bytes, not including the block_t structure. Negative for free block. */
        ssize_t m_size;
    } us_allocator_heap_block_t;

    /**< rounding_type is a pointer - so on 64 bit systems it is a 64 bits wide. If you have vectors, this neads to change */
    typedef void * us_allocator_heap_rounding_type;
    
    typedef struct us_allocator_heap_s
    {
        us_allocator_t m_base;

        /**< m_allocated is true if the data pointed to by m_base was allocated via new[] and needs to be freed by delete[] */
        bool m_allocated;
        
        /**< the rounded down size of the memory buffer */
        size_t m_size;
        
        /**< the base address of the memory buffer */
        void *m_memory_base;
        
        /**< the total count of allocated memory */
        size_t m_current_allocation_count;
        
        /**< the first block in the memory buffer */
        us_allocator_heap_block_t *m_first;
        
        /**< the last known free block in the memory buffer */
        us_allocator_heap_block_t *m_last_free;        
    } us_allocator_heap_t;

    bool us_allocator_heap_internal_init( us_allocator_heap_t *self );    

/**< create initial empty block in memory buffer */
    bool us_allocator_heap_init_raw( us_allocator_heap_t *self, void *base, size_t size );

    void us_allocator_heap_destroy( us_allocator_t *self_ );

    void * us_allocator_heap_alloc( us_allocator_t *self_, int32_t length, int32_t count );
    
    void * us_allocator_heap_realloc(
        us_allocator_t *self_,
        const void *orig_ptr,
        int32_t length,
        int32_t count
        );

    void us_allocator_heap_free( us_allocator_t *self_, const void *ptr );

/**< clear all blocks in the memory buffer, empty it out */   
    void us_allocator_heap_reset( us_allocator_heap_t *self );
    
/**< try allocate an arbitrary buffer */
    void *us_allocator_heap_internal_alloc ( us_allocator_heap_t *self, size_t size );
    
    void us_allocator_heap_internal_free ( us_allocator_heap_t *self, void *ptr );

    void us_allocator_heap_pack ( us_allocator_heap_t *self, us_allocator_heap_block_t *block );

    void us_allocator_heap_report( us_allocator_heap_t *self );        

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
