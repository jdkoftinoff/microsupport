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


int us_osc_dispatch_trie_star_comparator (
    us_trie_node_value_t a,
    us_trie_node_value_t b
)
{
    if ( a==(us_trie_node_value_t)('*')
            || b==(us_trie_node_value_t)('*') )
    {
        return 0;
    }
    else if ( a==(us_trie_node_value_t)('?') )
    {
        return 0;
    }
    else
    {
        return ( int ) ( a - b );
    }
}

int us_osc_dispatch_trie_star_skip (
    us_trie_node_value_t a
)
{
    int r;
    if ( a==(us_trie_node_value_t)('*') )
    {
        r=-1;
    }
    else
    {
        r=0;
    }
    return r;
}

bool us_osc_dispatch_init(
    us_osc_dispatch_t *self,
    us_allocator_t *allocator,
    int max_table_entries,
    int max_trie_elements
)
{
    bool r = false;
    self->allocator = allocator;
    self->trie = us_trie_dyn_create(
                     allocator,
                     max_trie_elements,
                     us_trie_basic_ignorer,
                     us_trie_basic_ignorer,
                     us_osc_dispatch_trie_star_skip,
                     us_osc_dispatch_trie_star_comparator
                 );
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

void us_osc_dispatch_destroy(us_osc_dispatch_t *self)
{
    if ( self->allocator )
    {
        if ( self->trie )
        {
            self->trie->destroy( self->trie );
        }
    }
}

bool us_osc_dispatch_add_entry(
    us_osc_dispatch_t *self,
    const char *address_prefix,
    const us_osc_dispatch_table_t *dispatch_table,
    const us_osc_dispatch_index_t *index_offset
)
{
    return us_osc_dispatch_add_entry_with_notify(
               self,
               address_prefix,
               dispatch_table,
               index_offset,
               0,
               0
           );
}

bool us_osc_dispatch_add_entry_with_notify(
    us_osc_dispatch_t *self,
    const char *address_prefix,
    const us_osc_dispatch_table_t *dispatch_table,
    const us_osc_dispatch_index_t *index_offset,
    bool (*notify_proc)(
        void *extra,
        const char *address,
        const us_osc_dispatch_table_t *table_item,
        const us_osc_dispatch_index_t *index
    ),
    void *extra
)
{
    bool r = false;
    if (self->map.num_entries < self->map.max_entries)
    {
        char resultant_address[256];
        int item = self->map.num_entries;
        us_osc_dispatch_map_entry_t *entry = &self->map.entries[ item ];
        r = true;
        entry->table_entry = dispatch_table;
        us_osc_dispatch_index_add(&entry->final_index, index_offset, &dispatch_table->index);
        strcpy(resultant_address, address_prefix);
        strcat(resultant_address, dispatch_table->address);
        /* if there is no notify proc then add the item to the trie.
         * If there is a notify proc, then add the item only if it returns true
         */
        if ( !notify_proc || notify_proc( extra, resultant_address, dispatch_table, &entry->final_index )==true )
        {
            us_trie_add(
                &self->trie->m_base,
                (us_trie_node_value_t *) resultant_address,
                strlen(resultant_address),
                (us_trie_node_flags_t) item
            );
            self->map.num_entries++;
            if (self->trie->m_base.m_num_nodes >= self->trie->m_base.m_max_nodes)
            {
                r = false;
            }
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
    return us_osc_dispatch_add_table_with_notify( self, address_prefix, dispatch_table, index_offset, 0, 0 );
}

bool us_osc_dispatch_add_table_with_notify(
    us_osc_dispatch_t *self,
    const char *address_prefix,
    const us_osc_dispatch_table_t dispatch_table[],
    const us_osc_dispatch_index_t *index_offset,
    bool (*notify_proc)(
        void *extra,
        const char *address,
        const us_osc_dispatch_table_t *table_item,
        const us_osc_dispatch_index_t *index
    ),
    void *extra
)
{
    bool r = true;
    while (dispatch_table->address != 0)
    {
        if (!us_osc_dispatch_add_entry_with_notify(
                    self,
                    address_prefix,
                    dispatch_table,
                    index_offset,
                    notify_proc,
                    extra
                ))
        {
            r = false;
            break;
        }
        dispatch_table++;
    }
    return r;
}


bool us_osc_dispatch_receive_msg(
    us_osc_dispatch_t *self,
    const us_osc_msg_t *msg,
    us_osc_sender_t *sender,
    void *extra
)
{
    bool r = false;
    us_trie_node_flags_t flags;
    int16_t match_len;
    us_trie_node_id_t match_item;
    if (us_trie_find(
                &self->trie->m_base,
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
                sender,
                extra
            );
    }
    return r;
}

