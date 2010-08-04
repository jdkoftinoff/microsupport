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
  * Neither the name of the <organization> nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

us_trie_t *
us_trie_init(
               us_trie_t *self,
               uint16_t max_nodes,
               uint16_t num_nodes,
               us_trie_node_t *nodes,
               us_trie_ignorer_proc ignorer,
               us_trie_comparator_proc comparator
               )
{
  self->m_max_nodes = max_nodes;
  self->m_num_nodes = num_nodes;
  self->m_nodes = nodes;
  self->m_ignorer = ignorer;
  self->m_comparator = comparator;
  self->m_first_free = 0;
  return self;
}

bool us_trie_basic_ignorer(
        us_trie_node_value_t v
        )
{
  return false;
}

int us_trie_basic_comparator(
        us_trie_node_value_t a,
        us_trie_node_value_t b
        )
{
  return (int)(a-b);
}


int
us_trie_count(
                const us_trie_t *self
                )
{
  us_trie_node_id_t cnt = self->m_max_nodes;
  us_trie_node_id_t last = 0;
  us_trie_node_t *buf = self->m_nodes;
  us_trie_node_id_t i;

  for (i = 0; i < cnt; ++i)
  {
    if (!us_trie_node_is_free( &buf[i] ))
      last = i;
  }
  return last + 1;
}


bool
us_trie_find_sibling(
                       const us_trie_t *self,
                       us_trie_node_id_t first_item,
                       us_trie_node_value_t v,
                       us_trie_node_id_t *item
                       )
{
  const us_trie_node_t *buf = self->m_nodes;

  while (! us_trie_node_is_free( &buf[ first_item ] ) )
  {
    /* does it match? */

    if ( !us_trie_node_is_end( &buf[ first_item ] ) &&
            self->m_comparator(buf[first_item].m_value, v) == 0)
    {
      /* yup! return the value */
      *item = first_item;
      return true;
    }
    else
    {
      /* nope! check his brother */
      if (buf[ first_item ].m_sibling == 0)
      {
        /* no more siblings, so return the last sibling and FALSE */
        *item = first_item;
        return false;
      }
      else
      {
        first_item = buf[ first_item ].m_sibling;
      }
    }
  }
  /* the caller must see if the item does match. If it doesn't,
     it is the index of the last sibling.
   */

  return false;
}

bool
us_trie_find(
        const us_trie_t *self,
        const us_trie_node_value_t *list,
        int16_t list_len,
        us_trie_node_flags_t *flags,
        int16_t *match_len,
        us_trie_node_id_t *match_item,
        us_trie_node_id_t initial_leaf_pos,
        us_trie_node_id_t initial_list_pos
        )
{
  us_trie_node_t *buf = self->m_nodes;
  us_trie_node_id_t i = initial_leaf_pos;
  us_trie_node_id_t list_pos = initial_list_pos;

  /* skip any initial items that are needed to ignore */

  while (list_pos < list_len && self->m_ignorer(list[list_pos]))
    ++list_pos;

  while (list_pos < list_len && !us_trie_node_is_free( &buf[i] ) )
  {
    if (!us_trie_find_sibling(self, i, list[list_pos], &i))
    {
      /* no match, return false */
      return false;
    }
    /* is there a child? */

    if (buf[i].m_child)
    {
      us_trie_node_id_t j;

      /* follow the child. */
      i = buf[i].m_child;

      /* if any of the children are an end point, then we have a match */
      for (j = i; j != 0; j = buf[j].m_sibling)
      {
        if ( us_trie_node_is_end( &buf[j] ) )
        {
          /*
           * found a match!
           * go through all children again and see if we can find a longer match
           */
          us_trie_node_id_t k;
          for (k = i; k != 0; k = buf[k].m_sibling)
          {
            if (us_trie_find(
                    self,
                    list,
                    list_len,
                    flags,
                    match_len,
                    match_item,
                    k,
                    list_pos + 1
                    ))
            {
              /* by definition it is longer than the one we have. */
              return true;
            }
          }

          /* our match is the longest */
          if (flags)
            *flags = us_trie_node_get_flags(&buf[j]);
          if (match_len)
            *match_len = list_pos + 1;
          if (match_item)
            *match_item = j;
          return true;
        }
      }

    }
    else
    {
      /* no, so we have a match. */
      if (flags)
        *flags = us_trie_node_get_flags(&buf[i]);
      if (match_len)
        *match_len = list_pos + 1;
      if (match_item)
        *match_item = i;
      return true;
    }

    /* move to the next non ignored item in the list */
    while (list_pos < list_len && self->m_ignorer(list[++list_pos]));
  }

  /* no match */
  return false;
}



int16_t
us_trie_extract(
                  const us_trie_t *self,
                  us_trie_node_value_t *list,
                  int16_t max_len,
                  us_trie_node_id_t end_leaf_index
                  )
{
  us_trie_node_t *buf = self->m_nodes;
  int16_t len = 0;
  us_trie_node_id_t cur_item = end_leaf_index;
  us_trie_node_value_t *p = list;

  if (end_leaf_index < 0)
  {
    return 0;
  }

  if ( us_trie_node_is_end( &buf[cur_item] ) )
  {
    cur_item = buf[cur_item].m_parent;
  }

  if (cur_item < 0)
  {
    return 0;
  }

  while (len < max_len)
  {
    *p++ = buf[ cur_item ].m_value;
    len++;

    if (buf[cur_item].m_parent == US_TRIE_NODE_EMPTY )
    {
      // found top of tree
      // now reverse the order
      if (len > 1)
      {
        uint16_t i;
        for ( i = 0; i < len / 2; ++i)
        {
          us_trie_node_value_t tmp = list[i];
          list[i] = list[len - i - 1];
          list[len - i - 1] = tmp;
        }
      }
      return len;
    }
    cur_item = buf[ cur_item ].m_parent;
  }
  return 0;
}





