#include "us_world.h"
#include "us_trie.h"

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

us_trie_dyn_t *
us_trie_dyn_create (
    us_allocator_t *allocator,
    uint16_t max_nodes,
    us_trie_ignorer_proc ignorer,
    us_trie_comparator_proc comparator
)
{
    us_trie_dyn_t *r = 0;
    us_trie_dyn_t *self = us_new ( allocator, us_trie_dyn_t );
    if ( self )
    {
        self->destroy = us_trie_dyn_destroy;
        self->m_allocator = allocator;
        self->m_nodes = us_new_array ( allocator, us_trie_node_t, max_nodes );
        if ( self->m_nodes )
        {
            uint16_t i;
            for ( i = 0; i < max_nodes; ++i )
            {
                us_trie_node_clear ( &self->m_nodes[i] );
            }
            us_trie_init (
                &self->m_base,
                max_nodes,
                0,
                self->m_nodes,
                ignorer,
                comparator
            );
            r = self;
        }
    }
    if ( r == 0 && self != 0 )
    {
        self->destroy ( self );
    }
    return r;
}

void
us_trie_dyn_destroy (
    us_trie_dyn_t *self
)
{
    if ( self->m_nodes != 0 )
    {
        us_delete ( self->m_allocator, self->m_nodes );
    }
    us_delete ( self->m_allocator, self );
}


void
us_trie_clear (
    us_trie_t *self
)
{
    us_trie_node_id_t i;
    for ( i = 0; i < self->m_max_nodes; ++i )
    {
        us_trie_node_clear ( &self->m_nodes[i] );
        self->m_first_free = 0;
        self->m_num_nodes = 0;
    }
}




us_trie_node_id_t
us_trie_add_child (
    us_trie_t *self,
    us_trie_node_id_t parent_item,
    us_trie_node_value_t value,
    us_trie_node_flags_t flags
)
{
    us_trie_node_id_t item = us_trie_find_next_free ( self );
    us_trie_node_t *buf = self->m_nodes;
    /* put data into found space, no further siblings, no children */
    buf[item].m_sibling = 0;
    buf[item].m_child = 0;
    buf[item].m_value = value;
    buf[item].m_flags = flags;
    buf[item].m_parent = parent_item;
    /* find last child */
    if ( parent_item != item && parent_item != -1 ) // but only if there are any parents at all!
    {
        int last_child = buf[parent_item].m_child;
        if ( last_child )
        {
            while ( buf[last_child].m_sibling )
            {
                last_child = buf[last_child].m_sibling;
            }
            // found the last child of the parent. Tell him he has a brother
            buf[last_child].m_sibling = item;
        }
        else
        {
            // parent has no children yet. add our new child as a child of parent
            if ( parent_item != -1 )
                buf[parent_item].m_child = item;
        }
    }
    if ( item >= self->m_num_nodes )
        self->m_num_nodes = item + 1;
    return item;
}



us_trie_node_id_t
us_trie_add_sibling (
    us_trie_t *self,
    us_trie_node_id_t last_sibling_item,
    us_trie_node_value_t value,
    us_trie_node_flags_t flags
)
{
    us_trie_node_id_t item = us_trie_find_next_free ( self );
    us_trie_node_t *buf = self->m_nodes;
    /* put data into found space, no further siblings, no children */
    buf[item].m_sibling = 0;
    buf[item].m_child = 0;
    buf[item].m_value = value;
    buf[item].m_flags = flags;
    buf[item].m_parent = buf[last_sibling_item].m_parent; /* same parent as my older brother */
    buf[last_sibling_item].m_sibling = item;
    if ( item > self->m_num_nodes )
        self->m_num_nodes = item;
    return item;
}



void
us_trie_remove (
    us_trie_t *self,
    us_trie_node_id_t item
)
{
    us_trie_node_release ( &self->m_nodes[item] );
    if ( item < self->m_first_free )
        self->m_first_free = item;
}




us_trie_node_id_t
us_trie_find_next_free (
    us_trie_t *self
)
{
    us_trie_node_id_t num = self->m_max_nodes;
    for ( ; self->m_first_free < num; ++self->m_first_free )
    {
        if ( us_trie_node_is_free ( &self->m_nodes[ self->m_first_free ] ) )
        {
            break;
        }
    }
    if ( self->m_first_free == num )
    {
        self->m_first_free = -1;
    }
    return self->m_first_free;
}


void
us_trie_add (
    us_trie_t *self,
    const us_trie_node_value_t *list,
    int16_t list_len,
    us_trie_node_flags_t flags
)
{
    /* is there a top family? */
    us_trie_node_id_t list_pos = 0;
    /* skip any initial items that are needed to ignore */
    while ( list_pos < list_len && self->m_ignorer ( list[list_pos] ) )
        ++list_pos;
    if ( us_trie_node_is_free ( &self->m_nodes[0] ) )
    {
        /* nope, this is our first ever add */
        us_trie_node_id_t i = -1;
        while ( list_pos < list_len )
        {
            /* add only the items that are not to be ignored */
            if ( !self->m_ignorer ( list[list_pos] ) )
            {
                i = us_trie_add_child ( self, i, list[list_pos], flags );
            }
            list_pos++;
        }
        i = us_trie_add_child ( self, i, 0, flags );
        us_trie_node_set_end ( &self->m_nodes[i] );
        /* and we are done! */
        return;
    }
    else
    {
        /* find a matching char */
        us_trie_node_id_t i = 0;
        while ( 1 )
        {
            /*  skip any items that are needed to ignore */
            while ( list_pos < list_len && self->m_ignorer ( list[list_pos] ) )
                ++list_pos;
            if ( list_pos == list_len )
            {
                i = us_trie_add_child ( self, self->m_nodes[i].m_parent, 0, flags );
                us_trie_node_set_end ( &self->m_nodes[i] );
                return; /* we are done! */
            }
            if ( us_trie_find_sibling ( self, i, list[list_pos], &i ) )
            {
                ++list_pos;
                /* We already have it */
                /* so traverse to the child of this sibling. */
                if ( self->m_nodes[i].m_child == 0 )
                {
                    /* no child, so add the rest of the children here */
                    while ( list_pos < list_len )
                    {
                        if ( !self->m_ignorer ( list[list_pos] ) )
                            i = us_trie_add_child ( self, i, list[list_pos], flags );
                        list_pos++;
                    }
                    i = us_trie_add_child ( self, i, 0, flags );
                    us_trie_node_set_end ( &self->m_nodes[i] );
                    /* and we are done! */
                    return;
                }
                else
                {
                    i = self->m_nodes[i].m_child;
                }
            }
            else
            {
                /* no sibling available */
                /* i points to last sibling of the family. */
                i = us_trie_add_sibling ( self, i, list[list_pos++], flags );
                while ( list_pos < list_len )
                {
                    if ( !self->m_ignorer ( list[list_pos] ) )
                        i = us_trie_add_child ( self, i, list[list_pos], flags );
                    list_pos++;
                }
                i = us_trie_add_child ( self, i, 0, flags );
                us_trie_node_set_end ( &self->m_nodes[i] );
                /* and we are done! */
                return;
            }
        }
    }
}




