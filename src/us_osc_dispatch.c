#include "us_world.h"

#include "us_osc_dispatch.h"

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

bool us_osc_dispatch_init(
    us_osc_dispatch_t *self,
    us_allocator_t *allocator,
    us_trie_t *trie,
    int max_table_entries
)
{
    bool r;
    self->allocator = allocator;
    self->trie = trie;
    self->destroy = us_osc_dispatch_destroy;
    self->receive_msg = us_osc_dispatch_receive_msg;
    self->map.entries = us_new_array(allocator, us_osc_dispatch_map_entry_t, max_table_entries);
    if (self->map.entries)
    {
        self->map.max_entries = max_table_entries;
        self->map.num_entries = 0;
        r = true;
    }
    return r;
}

void us_osc_dispatch_destroy(us_osc_dispatch_t *destroy)
{
    /* default do nothing */
}

bool us_osc_dispatch_add_entry(
    us_osc_dispatch_t *self,
    const char *address_prefix,
    const us_osc_dispatch_table_t *dispatch_table,
    const us_osc_dispatch_index_t *index_offset
)
{
    bool r = false;
    if (self->map.num_entries < self->map.max_entries)
    {
        char resultant_address[256];
        int item = self->map.num_entries++;
        us_osc_dispatch_map_entry_t *entry = &self->map.entries[ item ];
        r=true;
        entry->table_entry = dispatch_table;
        us_osc_dispatch_index_add(&entry->final_index, index_offset, &dispatch_table->index);
        strcpy(resultant_address, address_prefix);
        strcat(resultant_address, dispatch_table->address);
        us_trie_add(
            self->trie,
            (us_trie_node_value_t *) resultant_address,
            strlen(resultant_address),
            (us_trie_node_flags_t) item
        );
        if (self->trie->m_num_nodes >= self->trie->m_max_nodes)
        {
            r = false;
        }
    }
    return r;
}

bool us_osc_dispatch_add_table(
    us_osc_dispatch_t *self,
    const char *address_prefix,
    const us_osc_dispatch_table_t dispatch_table[],
    const us_osc_dispatch_index_t *index_offset
)
{
    bool r = true;
    while (dispatch_table->address != 0)
    {
        if (!us_osc_dispatch_add_entry(self, address_prefix, dispatch_table++, index_offset))
        {
            r = false;
            break;
        }
    }
    return r;
}

bool us_osc_dispatch_receive_msg(
    us_osc_dispatch_t *self,
    const us_osc_msg_t *msg,
    void *extra
)
{
    bool r = false;
    us_trie_node_flags_t flags;
    int16_t match_len;
    us_trie_node_id_t match_item;
    if (us_trie_find(
                self->trie,
                (us_trie_node_value_t *) msg->m_address,
                strlen(msg->m_address),
                &flags,
                &match_len,
                &match_item,
                0,
                0
            ))
    {
        us_osc_dispatch_map_entry_t *entry = &self->map.entries[ flags ];
        r = entry->table_entry->receive_msg_proc(
                self,
                msg,
                &entry->final_index,
                extra
            );
    }
    return r;
}

