#ifndef US_OSC_DISPATCH_H
#define US_OSC_DISPATCH_H

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

#include "us_world.h"

#include "us_allocator.h"
#include "us_osc_msg.h"
#include "us_osc_sender.h"
#include "us_trie.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** \addtogroup us_osc_dispatch OSC Message Dispatcher
     */
    /*@{*/

    struct us_osc_dispatch_s;
    struct us_osc_dispatch_index_s;
    struct us_osc_dispatch_table_s;

#define US_OSC_DISPATCH_INDEX_DIMS (4)


    int us_osc_dispatch_trie_star_comparator(
        us_trie_node_value_t a,
        us_trie_node_value_t b
    );

    int us_osc_dispatch_trie_star_skip (
        us_trie_node_value_t a
    );

    typedef bool (*us_osc_dispatch_proc_t)(
        struct us_osc_dispatch_s *self,
        const us_osc_msg_t *msg,
        const struct us_osc_dispatch_index_s *index,
        us_osc_sender_t *sender,
        void *extra
    );

    typedef bool (*us_osc_dispatch_notify_proc_t)(
        struct us_osc_dispatch_s *self,
        const char *address,
        const struct us_osc_dispatch_table_s *table_item,
        const struct us_osc_dispatch_index_s *index
    );

    typedef struct us_osc_dispatch_index_s
    {
        int axis[US_OSC_DISPATCH_INDEX_DIMS];
    } us_osc_dispatch_index_t;

    static inline void us_osc_dispatch_index_init(us_osc_dispatch_index_t *self)
    {
        int i;
        for (i = 0; i < US_OSC_DISPATCH_INDEX_DIMS; ++i)
            self->axis[i] = 0;
    }

    static inline void us_osc_dispatch_index_copy(
        us_osc_dispatch_index_t *self,
        const us_osc_dispatch_index_t *src
    )
    {
        int i;
        for (i = 0; i < US_OSC_DISPATCH_INDEX_DIMS; ++i)
            self->axis[i] = src->axis[i];
    }

    static inline void us_osc_dispatch_index_add(
        us_osc_dispatch_index_t *self,
        const us_osc_dispatch_index_t *src1,
        const us_osc_dispatch_index_t *src2
    )
    {
        int i;
        for (i = 0; i < US_OSC_DISPATCH_INDEX_DIMS; ++i)
            self->axis[i] = src1->axis[i] + src2->axis[i];
    }

    typedef struct us_osc_dispatch_table_s
    {
        const char *address;
        us_osc_dispatch_proc_t receive_msg_proc;
        us_osc_dispatch_index_t index;
    } us_osc_dispatch_table_t;

    typedef struct us_osc_dispatch_map_entry_s
    {
        const us_osc_dispatch_table_t *table_entry;
        us_osc_dispatch_index_t final_index;
    } us_osc_dispatch_map_entry_t;

    typedef struct us_osc_dispatch_map_s
    {
        us_osc_dispatch_map_entry_t *entries;
        int max_entries;
        int num_entries;
    } us_osc_dispatch_map_t;

    typedef struct us_osc_dispatch_s
    {
        void (*destroy)(struct us_osc_dispatch_s * self);
        bool (*receive_msg)(
            struct us_osc_dispatch_s *self,
            const us_osc_msg_t * msg,
            us_osc_sender_t *sender,
            void *extra
        );
        us_allocator_t *allocator;
        us_trie_dyn_t *trie;
        us_osc_dispatch_notify_proc_t notify;
        us_osc_dispatch_map_t map;
    } us_osc_dispatch_t;

    bool us_osc_dispatch_init(
        us_osc_dispatch_t *self,
        us_allocator_t *allocator,
        int max_table_entries,
        int max_trie_elements
    );

    void us_osc_dispatch_destroy(us_osc_dispatch_t *destroy);

    bool us_osc_dispatch_add_entry(
        us_osc_dispatch_t *self,
        const char *address_prefix,
        const us_osc_dispatch_table_t *dispatch_table,
        const us_osc_dispatch_index_t *index_offset
    );

    bool us_osc_dispatch_add_table(
        us_osc_dispatch_t *self,
        const char *address_prefix,
        const us_osc_dispatch_table_t dispatch_table[],
        const us_osc_dispatch_index_t *index_offset
    );

    bool us_osc_dispatch_add_entry_with_notify(
        us_osc_dispatch_t *self,
        const char *address_prefix,
        const us_osc_dispatch_table_t *dispatch_table,
        const us_osc_dispatch_index_t *index_offset,
        us_osc_dispatch_notify_proc_t notify_proc
    );

    bool us_osc_dispatch_add_table_with_notify(
        us_osc_dispatch_t *self,
        const char *address_prefix,
        const us_osc_dispatch_table_t dispatch_table[],
        const us_osc_dispatch_index_t *index_offset,
        us_osc_dispatch_notify_proc_t notify_proc
    );


    bool us_osc_dispatch_receive_msg(
        us_osc_dispatch_t *self,
        const us_osc_msg_t *msg,
        us_osc_sender_t *sender,
        void *extra
    );

    /*@}*/

#ifdef __cplusplus
}
#endif

#endif
