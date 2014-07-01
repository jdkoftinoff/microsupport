#ifndef US_TRIE_H
#define US_TRIE_H

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
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD..  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"

#include "us_allocator.h"
#ifdef __cplusplus
extern "C" {
#endif

#define US_TRIE_NODE_EMPTY ( int16_t )( -2 )
#define US_TRIE_NODE_NO_VALUE ( uint16_t )( 0x7fff )
#define US_TRIE_NODE_END_BIT ( uint16_t )( 0x8000 )

typedef int8_t us_trie_node_value_t;
typedef uint16_t us_trie_node_flags_t;
typedef int16_t us_trie_node_id_t;
typedef bool ( *us_trie_ignorer_proc )( us_trie_node_value_t );
typedef int ( *us_trie_db_skip_proc )( us_trie_node_value_t );
typedef int ( *us_trie_comparator_proc )( us_trie_node_value_t, us_trie_node_value_t );

bool us_trie_basic_ignorer( us_trie_node_value_t v );

int us_trie_basic_db_skip( us_trie_node_value_t v );

int us_trie_basic_comparator( us_trie_node_value_t a, us_trie_node_value_t b );

typedef struct us_trie_node_s
{
    us_trie_node_id_t m_parent;
    us_trie_node_id_t m_sibling;
    us_trie_node_id_t m_child;
    us_trie_node_value_t m_value;
    us_trie_node_flags_t m_flags;
} us_trie_node_t;

static inline void us_trie_node_clear( us_trie_node_t *self )
{
    self->m_sibling = 0;
    self->m_child = 0;
    self->m_parent = US_TRIE_NODE_EMPTY;
    self->m_flags = 0;
}

static inline bool us_trie_node_is_free( const us_trie_node_t *self )
{
    return ( self->m_parent == US_TRIE_NODE_EMPTY );
}

static inline void us_trie_node_release( us_trie_node_t *self )
{
    self->m_parent = US_TRIE_NODE_EMPTY;
}

static inline bool us_trie_node_is_end( const us_trie_node_t *self )
{
    return ( self->m_flags & US_TRIE_NODE_END_BIT ) != 0;
}

static inline void us_trie_node_set_end( us_trie_node_t *self )
{
    self->m_flags |= US_TRIE_NODE_END_BIT;
}

static inline void us_trie_node_unset_end( us_trie_node_t *self )
{
    self->m_flags &= ~US_TRIE_NODE_END_BIT;
}

static inline us_trie_node_flags_t us_trie_node_get_flags( const us_trie_node_t *self )
{
    return self->m_flags & ( ~US_TRIE_NODE_END_BIT );
}

static inline void us_trie_node_set_flags( us_trie_node_t *self, us_trie_node_flags_t f )
{
    self->m_flags = ( self->m_flags & US_TRIE_NODE_END_BIT ) | ( f & ~US_TRIE_NODE_END_BIT );
}

typedef struct us_trie_s
{
    us_trie_node_id_t m_max_nodes;
    us_trie_node_id_t m_num_nodes;
    us_trie_node_t *m_nodes;
    us_trie_node_id_t m_first_free;
    us_trie_ignorer_proc m_query_ignorer;
    us_trie_ignorer_proc m_db_ignorer;
    us_trie_db_skip_proc m_db_skip;
    us_trie_comparator_proc m_comparator;
} us_trie_t;

typedef struct us_trie_dyn_s
{
    us_trie_t m_base;
    void ( *destroy )( struct us_trie_dyn_s * );
    us_allocator_t *m_allocator;
    us_trie_node_t *m_nodes;
} us_trie_dyn_t;

us_trie_dyn_t *us_trie_dyn_create( us_allocator_t *allocator,
                                   uint16_t max_nodes,
                                   us_trie_ignorer_proc query_ignorer,
                                   us_trie_ignorer_proc db_ignorer,
                                   us_trie_db_skip_proc db_skip,
                                   us_trie_comparator_proc comparator );

void us_trie_dyn_destroy( us_trie_dyn_t *self );

/** Initialize a trie with pre-allocated nodes and us_trie_t structure.

@param self trie to initialize
@param max_nodes maximum nodes in nodes list
@param num_nodes current number of active nodes in nodes list
@param nodes nodes list
@param ignorer Function to call to test for ignored characters
@param comparator Function to call to compare characters

@return self
*/

us_trie_t *us_trie_init( us_trie_t *self,
                         uint16_t max_nodes,
                         uint16_t num_nodes,
                         us_trie_node_t *nodes,
                         us_trie_ignorer_proc query_ignorer,
                         us_trie_ignorer_proc db_ignorer,
                         us_trie_db_skip_proc db_skip,
                         us_trie_comparator_proc comparator );

void us_trie_clear( us_trie_t *self );

int us_trie_count( const us_trie_t *self );

static inline const us_trie_node_t *us_trie_get_node( const us_trie_t *self, us_trie_node_id_t num )
{
    return &self->m_nodes[num];
}

us_trie_node_id_t
    us_trie_add_child( us_trie_t *self, us_trie_node_id_t parent_item, us_trie_node_value_t value, us_trie_node_flags_t flags );

us_trie_node_id_t us_trie_add_sibling( us_trie_t *self,
                                       us_trie_node_id_t last_sibling_item,
                                       us_trie_node_value_t value,
                                       us_trie_node_flags_t flags );

void us_trie_remove( us_trie_t *self, us_trie_node_id_t item );

us_trie_node_id_t us_trie_find_next_free( us_trie_t *self );

bool us_trie_find_sibling( const us_trie_t *self,
                           us_trie_node_id_t first_item,
                           us_trie_node_value_t value,
                           us_trie_node_id_t *item );
void us_trie_add( us_trie_t *self, const us_trie_node_value_t *list, int16_t list_len, us_trie_node_flags_t flags );

bool us_trie_find( const us_trie_t *self,
                   const us_trie_node_value_t *list,
                   int16_t list_len,
                   us_trie_node_flags_t *flags,
                   int16_t *match_len,
                   us_trie_node_id_t *match_item,
                   us_trie_node_id_t initial_leaf_pos,
                   us_trie_node_id_t initial_list_pos );

int16_t us_trie_extract( const us_trie_t *self, us_trie_node_value_t *list, int16_t max_len, us_trie_node_id_t end_leaf_index );

#ifdef __cplusplus
}
#endif

#endif
