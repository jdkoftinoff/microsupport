#include "us_world.h"

#include "us_allocator.h"
#include "us_allocator_heap.h"

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
  * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define US_HEAP_MAGIC (0x3141592)

/**< us_allocator_heap_rounding_type_size is just the size of the largest fundamental type on this system */
static const size_t us_allocator_heap_rounding_type_size = sizeof(us_allocator_heap_rounding_type);

/**< us_allocator_rounded_block_size is the block_t structure size rounded to the next largest rounding_type boundary. */
static const size_t us_allocator_rounded_block_size
    = ((sizeof(us_allocator_heap_block_t) + (sizeof(us_allocator_heap_rounding_type) - 1))
       / sizeof(us_allocator_heap_rounding_type)) * sizeof(us_allocator_heap_rounding_type);

/**<  round up a byte size to the nearest larger alignment */
static inline size_t us_allocator_round_up_bytes(size_t a) {
    return ((a + us_allocator_heap_rounding_type_size - 1) / us_allocator_heap_rounding_type_size)
           * us_allocator_heap_rounding_type_size;
}

/**< round down a byte size to the nearest smaller alignment */
static inline size_t us_allocator_round_down_types(size_t a) { return a & ~(us_allocator_heap_rounding_type_size - 1); }

void us_allocator_heap_validate_block(us_allocator_heap_block_t *block) {
    if ((uint64_t)block < 0x1000 && block != 0) {
        us_log_error("block is bad");
        abort();
    }
    if (block) {
        if (block->m_magic != US_HEAP_MAGIC) {
            us_log_error("block->m_magic is bad");
            abort();
        }
        if ((uint64_t)block->m_next < 0x1000 && (uint64_t)block->m_next != 0) {
            us_log_error("block->m_next is bad");
            abort();
        }
        if ((uint64_t)block->m_prev < 0x1000 && (uint64_t)block->m_prev != 0) {
            us_log_error("block->m_prev is bad");
            abort();
        }
        if (((uint64_t)block->m_prev != 0) && (block->m_prev->m_next != block)) {
            us_log_error("block->m_prev->m_next is bad");
            abort();
        }
        if (((uint64_t)block->m_next != 0) && (block->m_next->m_prev != block)) {
            us_log_error("block->m_next->m_prev is bad");
            abort();
        }
    }
}

/**< create initial empty block in memory buffer */
bool us_allocator_heap_internal_init(us_allocator_heap_t *self) {
    /* clear all allocations */
    self->m_current_allocation_count = 0;
    /* the first block is at the beginning of the buffer */
    self->m_first = (us_allocator_heap_block_t *)(self->m_memory_base);
    /* set the first block entries, it is the only block and it is empty */
    self->m_first->m_magic = US_HEAP_MAGIC;
    self->m_first->m_next = 0;
    self->m_first->m_prev = 0;
    self->m_first->m_size
        = (size_t)(-(ssize_t)(self->m_size - us_allocator_rounded_block_size)); /* negative size means empty */
    /* the first empty block is also our last free block */
    self->m_last_free = self->m_first;
    return true;
}

bool us_allocator_heap_init_raw(us_allocator_heap_t *self, void *base, size_t size) {
    self->m_allocated = false;
    self->m_size = us_allocator_round_down_types(size);
    self->m_memory_base = base;
    self->m_base.destroy = us_allocator_heap_destroy;
    self->m_base.alloc = us_allocator_heap_alloc;
    self->m_base.realloc = us_allocator_heap_realloc;
    self->m_base.free = us_allocator_heap_free;
    return us_allocator_heap_internal_init(self);
}

void us_allocator_heap_destroy(us_allocator_t *self_) {
    us_allocator_heap_t *self = (us_allocator_heap_t *)self_;
    if (self->m_allocated) {
        free(self->m_memory_base);
        self->m_memory_base = 0;
    }
}

void *us_allocator_heap_alloc(us_allocator_t *self_, int32_t length, int32_t count) {
    us_allocator_heap_t *self = (us_allocator_heap_t *)self_;
    us_allocator_heap_validate(self);
    return (us_allocator_heap_internal_alloc(self, us_round_size(length * count)));
}

void *us_allocator_heap_realloc(us_allocator_t *self_, const void *orig_ptr, int32_t length, int32_t count) {
    void *new_ptr = self_->alloc(self_, length, count);
    if (new_ptr) {
        memcpy(new_ptr, orig_ptr, us_round_size(length * count));
        self_->free(self_, orig_ptr);
    }
    return new_ptr;
}

void us_allocator_heap_free(us_allocator_t *self_, const void *ptr) {
    us_allocator_heap_t *self = (us_allocator_heap_t *)self_;
    us_allocator_heap_validate(self);
    us_allocator_heap_internal_free(self, (void *)ptr);
}

/**< clear all blocks in the memory buffer, empty it out */
void us_allocator_heap_reset(us_allocator_heap_t *self) { us_allocator_heap_internal_init(self); }

void *us_allocator_heap_internal_alloc(us_allocator_heap_t *self, size_t size) {
    us_allocator_heap_block_t *cur = self->m_last_free;
    us_allocator_heap_block_t *usable = 0;
    void *ptr = 0;
    /* find a free memory block that is big enough */
    if (!cur) {
        cur = self->m_first;
    }
    /* allow for allocation of 0 sized amounts */
    if (size == 0) {
        size = us_allocator_heap_rounding_type_size;
    }
    /* round the size to the nearest larger alignment size */
    size = us_allocator_round_up_bytes(size);
    do {
        if ((cur->m_size) < 0 && (size_t)(-(cur->m_size)) >= size) {
            /* found a free memory block big enough! */
            usable = cur;
            break;
        }
        cur = cur->m_next;
    } while (cur != NULL);
    /* if cur is NULL, we couldn't find ANY free blocks big enough */
    if (cur == NULL) {
        /* We need to pack all free memory. Iterate through all blocks and call pack() on them. */
        cur = self->m_first;
        while (cur != NULL) {
            us_allocator_heap_pack(self, cur);
            cur = cur->m_next;
        }
        /* now try find a free block that may be large enough */
        cur = self->m_first;
        if (cur) {
            do {
                if ((cur->m_size) < 0 && (size_t)(-(cur->m_size)) >= size) {
                    /* found a free memory block big enough! */
                    usable = cur;
                    break;
                }
                cur = cur->m_next;
            } while (cur != NULL);
        }
        if (cur == NULL) {
            return 0; /* no memory! return 0 */
        }
    }
    /*
       we have found a big enough block. Check to see if we should
       split this block into two blocks so we don't waste memory
       Only split the block if the block is more than128 bytes larger
       than the requested amount
    */
    usable->m_size = -usable->m_size; /* mark the block as not free. */
    self->m_current_allocation_count += usable->m_size;
    if ((size_t)(usable->m_size) > (size + 128)) {
        /* yes, we should split the block into two free blocks */
        size_t orig_size = usable->m_size;
        us_allocator_heap_block_t *orig_next = usable->m_next;
        /* Make the block perfectly sized */
        usable->m_size = (ssize_t)(size);
        self->m_current_allocation_count -= (orig_size - size);
        /* calculate the position of the next block */
        usable->m_next = (us_allocator_heap_block_t *)((unsigned long long)(usable) + size + us_allocator_rounded_block_size);
        /* put the links in properly */
        usable->m_next->m_next = orig_next;
        usable->m_next->m_prev = usable;
        usable->m_next->m_magic = US_HEAP_MAGIC;
        if (orig_next)
            orig_next->m_prev = usable->m_next;
        /* figure out how big the left over block is */
        usable->m_next->m_size = -((ssize_t)(orig_size) - (ssize_t)(size) - (ssize_t)(us_allocator_rounded_block_size));
        self->m_last_free = usable->m_next;
        us_allocator_heap_validate_block(usable);
        us_allocator_heap_validate_block(usable->m_next);
        us_allocator_heap_validate_block(usable->m_prev);
    }
    ptr = (void *)((unsigned long long)(usable) + us_allocator_rounded_block_size);
    /* return the data section of the block to the caller */
    return ptr;
}

void us_allocator_heap_internal_free(us_allocator_heap_t *self, void *ptr) {
    us_allocator_heap_pack(self, self->m_first);
    /* only free non-null pointers */
    us_allocator_heap_validate(self);
    if (ptr != 0) {
        /*
           subtract the MemBlock header size from the pointer given to us
           to get the MemBlock header address
        */
        us_allocator_heap_block_t *block
            = (us_allocator_heap_block_t *)(((unsigned long long)ptr) - us_allocator_rounded_block_size);
        if (block->m_magic != US_HEAP_MAGIC) {
            us_log_error("bad magic");
            abort();
        }
        us_allocator_heap_validate_block(block);
        /* mark it as free by changing the size to negative size */
        if (block->m_size > 0) {
            ssize_t sz = block->m_size;
            self->m_current_allocation_count -= sz;
            block->m_size = -sz;
        } else {
            us_log_error("attempt to double free a memory heap block");
        }
        /* call pack() to pack it and any free blocks before and after it */
        us_allocator_heap_pack(self, block);
    }
}

void us_allocator_heap_pack(us_allocator_heap_t *self, us_allocator_heap_block_t *block) {
    us_allocator_heap_block_t *first = NULL;
    us_allocator_heap_block_t *cur = NULL;
    us_allocator_heap_block_t *last = NULL;
    cur = block;
    /* block had better be already free before calling this! */
    us_allocator_heap_validate(self);
    if ((cur->m_size) < 0) {
        /* search backwards for contiguous free blocks */
        while (cur->m_prev != NULL && (cur->m_prev->m_size) < 0) {
            us_allocator_heap_validate_block(cur);
            cur = cur->m_prev;
        }
        first = cur;
        /* update the heaps last free block cache so it does not get messed up */
        self->m_last_free = first;
        /* search forward for contiguous free blocks */
        cur = block;
        while (cur->m_next != NULL && (cur->m_next->m_size) < 0) {
            us_allocator_heap_validate_block(cur);
            cur = cur->m_next;
        }
        last = cur;
        /*
           if there is not only one lonely block of free memory,
           there is space to reclaim
        */
        if (first != last) {
            /*
               calculate size of reclaimed space
               It is the difference between the first block
               address and the last block address, plus the
               size of the last block.
               This value is then the size of the whole
               free area including the us_allocator_heap_block_t header on the
               first block. We subtract this from our total
               and set the first block size to the negative
               of that, since it is a free block
            */
            first->m_size = -(((char *)(last) - (char *)(first)) + (-(last->m_size)));
            /* fix links */
            first->m_next = last->m_next;
            if (last->m_next != NULL) {
                last->m_next->m_prev = first;
            }
            us_allocator_heap_validate_block(first);
            us_allocator_heap_validate_block(first->m_next);
            us_allocator_heap_validate_block(first->m_prev);
        }
    }
}

void us_allocator_heap_report(us_allocator_heap_t *self) {
    uint32_t free_chunks = 0;
    uint32_t used_chunks = 0;
    ssize_t largest_free = 0;
    ssize_t largest_used = 0;
    size_t free_mem = 0;
    size_t used_mem = 0;
    us_allocator_heap_block_t *cur = self->m_first;
    us_log_info("us_allocator_heap_t dump");
    while (cur != NULL) {
        ssize_t sz = cur->m_size;
        us_log_info("heap block %p : next=%p, prev=%p, size=%d", cur, cur->m_next, cur->m_prev, cur->m_size);
        if (sz < 0) {
            free_mem += -sz;
            ++free_chunks;
            if (-sz > largest_free)
                largest_free = -sz;
        } else {
            used_mem += sz;
            ++used_chunks;
            if (sz > largest_used)
                largest_used = sz;
        }
        cur = cur->m_next;
    }
    us_log_info("size of largest free chunk: %d", largest_free);
    us_log_info("size of largest used chunk: %d ", largest_used);
    us_log_info("total free mem: %d ", free_mem);
    us_log_info("total used mem: %d", used_mem);
    us_log_info("free chunk count: %d", free_chunks);
    us_log_info("used chunk count: %d", used_chunks);
}

void us_allocator_heap_validate(us_allocator_heap_t *self) {
    us_allocator_heap_block_t *cur = self->m_first;
    while (cur != NULL) {
        us_allocator_heap_validate_block(cur);
        cur = cur->m_next;
    }
}
