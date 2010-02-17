#include "us_world.h"
#include "us_buf.h"

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

void us_buf_init(
                    us_buf_t *self,
                    uint8_t *buf,
                    int buf_size
                    )
{
  self->m_buf = buf;
  self->m_buf_size = buf_size;
  self->m_next_in = 0;
  self->m_next_out = 0;
}

int us_buf_readable_count( us_buf_t *self )
{
  int c = self->m_next_in - self->m_next_out;
  if( c<0 )
  {
    c+=self->m_buf_size;
  }
  return c;
}

void us_buf_read( us_buf_t *self, uint8_t *dest_data, int dest_data_cnt )
{
  int i;
  for( i=0; i<dest_data_cnt; ++i )
  {
    dest_data[ i ] = self->m_buf[ self->m_next_out ];
    if( ++self->m_next_out == self->m_buf_size )
    {
      self->m_next_out = 0;
    }
  }
}

int us_buf_writeable_count( us_buf_t *self )
{
  int c = us_buf_readable_count(self);
  return self->m_buf_size - c - 1;
}

void us_buf_write( us_buf_t *self, uint8_t *src_data, int src_data_cnt )
{
  int i;
  for (i=0; i<src_data_cnt; i++)
  {
    self->m_buf[ self->m_next_in ] = src_data[i];
    if( ++self->m_next_in == self->m_buf_size )
    {
      self->m_next_in=0;
    }
  }
}
