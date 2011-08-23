#include "us_world.h"

#include "us_logger_printer.h"

#include "us_allocator_heap.h"


#include "us_testutil.h"

#include <limits.h>

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
    * Neither the name of Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC.  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/** \addtogroup us_test_allocator_heap */
/*@{*/

int32_t us_test_allocator_heap_get_largest_free_size( us_allocator_heap_t *self )
{
    uint32_t free_chunks = 0;
    uint32_t used_chunks = 0;
    ssize_t largest_free = 0;
    ssize_t largest_used = 0;
    size_t free_mem = 0;
    size_t used_mem = 0;
    us_allocator_heap_block_t *cur = self->m_first;
    while ( cur != NULL )
    {
        ssize_t sz = cur->m_size;
        if ( sz < 0 )
        {
            free_mem += -sz;
            ++free_chunks;
            if ( -sz > largest_free )
                largest_free = -sz;
        }
        else
        {
            used_mem += sz;
            ++used_chunks;
            if ( sz > largest_used )
                largest_used = sz;
        }
        cur = cur->m_next;
    }
    return largest_free;
}
int saved_allocation_item[10000];
int saved_allocation_size[10000];
bool saved_allocation_alloc_or_free[10000];
int saved_repeat_count;

int min_allocation_item[10000];
int min_allocation_size[10000];
bool min_allocation_alloc_or_free[10000];
int min_repeat_count=INT_MAX;

#define MAX_ALLOCATIONS (256)
size_t min_allocated=INT_MAX;
bool us_test_allocator_heap_stress( us_allocator_heap_t *allocator )
{
    void *allocations[MAX_ALLOCATIONS];
    int repeat_count=10000;
    int i;
    for ( i=0; i<MAX_ALLOCATIONS; ++i )
    {
        allocations[i] = 0;
    }
    for ( i=0; (i<repeat_count) && (i<min_repeat_count) ; ++i )
    {
        bool alloc_or_free = rand() & 1;
        int item = rand() % MAX_ALLOCATIONS;
        int sz = (rand() % 256)+4;
        saved_allocation_item[i]=item;
        saved_allocation_alloc_or_free[i]=alloc_or_free;
        saved_allocation_size[i]=sz;
        saved_repeat_count=i;
        if ( alloc_or_free )
        {
            int largest_free;
            if ( allocations[item] )
            {
                us_delete( &allocator->m_base, allocations[item] );
                allocations[item]=0;
            }
            allocations[item] = allocator->m_base.alloc( &allocator->m_base, sz, 1 );
            largest_free =us_test_allocator_heap_get_largest_free_size( allocator );
            if ( allocations[item] == 0 && largest_free>sz )
            {
                us_log_error( "Tried to allocate %d bytes, failed, but largest free block was %d", sz, largest_free );
#if 1
                us_allocator_heap_report( allocator );
                allocations[item] = allocator->m_base.alloc( &allocator->m_base, sz, 1 );
                abort();
#else
                return(false);
#endif
            }
            else if (allocations[item] == 0)
            {
                if ( allocator->m_current_allocation_count < min_allocated)
                {
                    min_allocated = allocator->m_current_allocation_count;
                    us_log_info( "Tried to allocate %d bytes, failed, largest free block was %d, new min %d", sz, largest_free, min_allocated );
                    us_allocator_heap_report( allocator );
                }
            }
        }
        else
        {
            if ( allocations[item] )
            {
                us_delete( &allocator->m_base, allocations[item] );
                allocations[item]=0;
            }
        }
    }
    for ( i=0; i<MAX_ALLOCATIONS; ++i )
    {
        us_delete( &allocator->m_base, allocations[i] );
        allocations[i]=0;
    }
    return true;
}

int us_test_allocator_heap_main ( int argc, char **argv )
{
    int i,j;
    bool r = true;
    static uint32_t raw_memory[0x8000];
    us_allocator_heap_t heap;
    if ( us_testutil_start ( 2048, 2048, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
#if 1
        j=0;
        while (j<1)
        {
            for (i=0; i<sizeof(raw_memory)/sizeof(raw_memory[0]); i++)
            {
                raw_memory[i]=0;
            }
            if ( us_allocator_heap_init_raw( &heap, &raw_memory[0], sizeof(raw_memory) ) )
            {
                r = us_test_allocator_heap_stress( &heap );
                if (!r && (saved_repeat_count<min_repeat_count))
                {
                    for (i=0; i<=saved_repeat_count; i++)
                    {
                        min_allocation_item[i]=saved_allocation_item[i];
                        min_allocation_size[i]=saved_allocation_size[i];
                        min_allocation_alloc_or_free[i]=saved_allocation_alloc_or_free[i];
                    }
                    min_repeat_count=saved_repeat_count;
                    us_log_info ( "new min (%d) after %d\n",min_repeat_count,j);
                    if (min_repeat_count==0)
                    {
                        break;
                    }
                }
            }
            j++;
        }
//        for(i=0; i<=min_repeat_count; i++)
//        {
//            us_log_info ( "item[%d],size=%d, %s\n", min_allocation_item[i],min_allocation_size[i],min_allocation_alloc_or_free[i]?"alloc":"free");
//        }
#else
        if ( us_allocator_heap_init_raw( &heap, &raw_memory[0], sizeof(raw_memory) ) )
        {
            void * ptr1,*ptr2,*ptr3;
            ptr1=(void *)heap.m_base.alloc( &heap.m_base, 253, 1 );
            ptr2=(void *)heap.m_base.alloc( &heap.m_base, 181, 1 );
            us_delete( &heap.m_base, ptr1 );
            ptr3=(void *)heap.m_base.alloc( &heap.m_base, 175, 1 );
            if (ptr1==0)
            {
                us_log_error( "ptr1 failed\n");
            }
            if (ptr2==0)
            {
                us_log_error( "ptr2 failed\n");
            }
            if (ptr3==0)
            {
                us_log_error( "ptr3 failed\n");
            }
        }
        else
        {
            us_log_error( "init failed\n");
        }
#endif
        us_log_info ( "Finishing us_test_allocator" );
        us_logger_finish();
        us_testutil_finish();
    }
    return r ? 0:1;
}

/*@}*/

