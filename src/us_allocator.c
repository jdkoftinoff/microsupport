#include "us_world.h"

#include "us_allocator.h"

#include "us_print.h"
#include "us_logger_syslog.h"

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

us_allocator_t *
us_simple_allocator_init(
                             us_allocator_t *self,
                             void *raw_memory,
                             int32_t raw_memory_length
                             )
{
  self->m_raw_memory = raw_memory;
  self->m_raw_memory_length = raw_memory_length;
  self->m_current_position = 0;
  self->destroy = us_simple_allocator_destroy;
  self->alloc = us_simple_allocator_alloc;
  return self;
}

void us_simple_allocator_destroy(
                                     us_allocator_t *self
                                     )
{
  /* Do Nothing */
}

void *us_simple_allocator_alloc(
                                    us_allocator_t *self,
                                    int32_t length,
                                    int32_t count
                                    )
{
  void *result = 0;
  /* round up total size to 32 bit alignment */
  int32_t total_size_to_allocate = us_round_size( length*count );

  if( (self->m_current_position + total_size_to_allocate)
     <= (self->m_raw_memory_length) )
  {
    result = ((char *)self->m_raw_memory) + self->m_current_position;
    self->m_current_position += total_size_to_allocate;
  }
  return result;
}

void
us_allocator_reset(
                       us_allocator_t *self
                       )
{
  self->m_current_position=0;
}


#if US_ENABLE_MALLOC

us_allocator_t *
us_malloc_allocator_init(
                             us_allocator_t *self,
                             int32_t raw_memory_length
                             )
{
  self->m_raw_memory = US_DEFAULT_MALLOC( (size_t)raw_memory_length );
  self->m_raw_memory_length = raw_memory_length;
  self->m_current_position = 0;
  self->destroy = us_malloc_allocator_destroy;
  self->alloc = us_simple_allocator_alloc;
  return self;
}

void us_malloc_allocator_destroy(
                                     us_allocator_t *self
                                     )
{
  US_DEFAULT_FREE( self->m_raw_memory );
}
#endif

