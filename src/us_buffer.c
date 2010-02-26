#include "us_world.h"

#include "us_buffer.h"
#include "us_buffer_print.h"

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

bool us_buffer_append_byte(
        us_buffer_t *self,
        int value
        )
{
  uint8_t byte_value = (uint8_t)value;
  return self->append(self, &byte_value, 1);
}

bool us_buffer_read_byte(
                             us_buffer_t *self,
                             void *value_ptr_
                             )
{
  bool r=false;
  uint8_t *value_ptr = (uint8_t *)value_ptr_;

  if( self->m_cur_read_pos < self->m_cur_length )
  {
    *value_ptr = self->m_buffer[ self->m_cur_read_pos++ ];
    r=true;
  }
  return r;
}

bool us_buffer_append_string(
        us_buffer_t *self,
        const char *str
        )
{
  bool r = true;
  int32_t len = (int32_t)strlen(str);
  r &= self->append(self, str, len);
  return r;
}

bool us_buffer_read_string(
                               us_buffer_t *self,
                               char *value_ptr,
                               int32_t result_max_len
                               )
{
  bool r=false;

  if( self->m_cur_read_pos < self->m_cur_length )
  {
    /* copy up to the maximum length of string */
    strncpy( value_ptr, us_buffer_read_ptr(self), result_max_len );

    /* if the string fit then the last possible char in the result buffer is zero */
    if( value_ptr[ result_max_len-1 ] == '\0' )
    {
      int len=strlen( value_ptr );
      /* update read ptr */
      self->m_cur_read_pos += (len + 1);
      r=true;
    }
  }
  return r;
}


bool us_buffer_append_rounded_string(
        us_buffer_t *self,
        const char *str
        )
{
  bool r = true;
  int nul_count = 0;
  int i;
  int32_t len = (int32_t)strlen(str);
  r &= self->append(self, str, len);

  // length -> padding
  // 4 -> 4
  // 5 -> 3
  // 6 -> 2
  // 7 -> 1
  // 8 -> 4

  nul_count = 4 - (len & 3);

  for (i = 0; i < nul_count && r == true; ++i)
  {
    r &= us_buffer_append_byte(self, 0);
  }

  return r;
}

bool us_buffer_read_rounded_string(
                                       us_buffer_t *self,
                                       char *value,
                                       int32_t result_max_len
                                       )
{
  bool r=false;

  r = us_buffer_read_string(
                                self,
                                value,
                                result_max_len
                                );
  if( r )
  {
    /* account for extra padding */

    while( (self->m_cur_read_pos&3) != 0 )
    {
      self->m_cur_read_pos++;
    }
  }
  return r;
}


bool us_buffer_append_int32(
        us_buffer_t *self,
        int32_t value
        )
{
  bool r = true;
  r &= us_buffer_append_byte(self, US_GET_BYTE_3(value) );
  r &= us_buffer_append_byte(self, US_GET_BYTE_2(value) );
  r &= us_buffer_append_byte(self, US_GET_BYTE_1(value) );
  r &= us_buffer_append_byte(self, US_GET_BYTE_0(value) );
  return r;
}


bool us_buffer_read_int32(
                              us_buffer_t *self,
                              int32_t *value_ptr
                              )
{
  bool r=false;
  int32_t value = 0;

  if( self->m_cur_read_pos+3 < self->m_cur_length )
  {
    value = (self->m_buffer[ self->m_cur_read_pos++ ])<<24;
    value |= (self->m_buffer[ self->m_cur_read_pos++ ])<<16;
    value |= (self->m_buffer[ self->m_cur_read_pos++ ])<<8;
    value |= (self->m_buffer[ self->m_cur_read_pos++ ])<<0;
    *value_ptr = value;
    r=true;
  }

  return r;
}


bool us_buffer_append_uint64(
        us_buffer_t *self,
        uint32_t value_high,
        uint32_t value_low
        )
{
  bool r = true;
  r &= us_buffer_append_int32(
                                  self,
                                  value_high
                                  );
  r &= us_buffer_append_int32(
                                  self,
                                  value_low
                                  );
  return r;
}

bool us_buffer_read_uint64(
                               us_buffer_t *self,
                               uint32_t *value_high_ptr,
                               uint32_t *value_low_ptr
                               )
{
  bool r=true;

  r&=us_buffer_read_int32( self, (int32_t *)value_high_ptr );
  if( r )
  {
    r&=us_buffer_read_int32( self, (int32_t *)value_low_ptr );
  }
  return r;
}

bool us_buffer_append_float32(
        us_buffer_t *self,
        float value
        )
{
  bool r = true;
  float *value_ptr = &value;
  int32_t int_value = *((int32_t *) value_ptr);
  r &= us_buffer_append_int32(
          self,
          int_value
          );
  return r;
}


bool us_buffer_read_float32(
                                us_buffer_t *self,
                                float *value_ptr
                                )
{
  bool r=true;

  r&=us_buffer_read_int32( self, (int32_t *)value_ptr );

  /* check for NaN and refuse to accept it */
  if( *value_ptr != *value_ptr )
    r=false;

  return r;
}


#if US_ENABLE_DOUBLE

union us_float64_splitter_u
{
  double value;
  uint32_t ints[2];
};

bool us_buffer_append_float64(
        us_buffer_t *self,
        double value
        )
{
  bool r = true;
  union us_float64_splitter_u splitter;
  uint32_t low_int_value;
  uint32_t high_int_value;

  splitter.value = value;

#if US_BIG_ENDIAN
  low_int_value = splitter.ints[0];
  high_int_value = splitter.ints[1];
#else
  low_int_value = splitter.ints[1];
  high_int_value = splitter.ints[0];
#endif
  r &= us_buffer_append_int32(
          self,
          high_int_value
          );
  r &= us_buffer_append_int32(
          self,
          low_int_value
          );
  return r;
}

bool us_buffer_read_float64(
                                us_buffer_t *self,
                                double *value_ptr
                                )
{
  bool r=true;
  union us_float64_splitter_u splitter;
  uint32_t *low_int_value_ptr;
  uint32_t *high_int_value_ptr;

#if US_BIG_ENDIAN
  low_int_value_ptr = &splitter.ints[0];
  high_int_value_ptr = &splitter.ints[1];
#else
  low_int_value_ptr = &splitter.ints[1];
  high_int_value_ptr = &splitter.ints[0];
#endif

  r&=us_buffer_read_uint64(self, high_int_value_ptr, low_int_value_ptr );

  if( r )
  {
    *value_ptr = splitter.value;

    /* check for NaN and refuse to accept it */
    if( *value_ptr != *value_ptr )
      r=false;
  }
  return r;
}

#endif

bool us_buffer_append_data(
        us_buffer_t *self,
        const void *data,
        int32_t data_length
        )
{
  bool r = true;
  r &= self->append(self, data, data_length);
  return r;
}

bool us_buffer_read_data(
                             us_buffer_t *self,
                             void *data,
                             int32_t max_data_length,
                             int32_t *data_length
                             )
{
  bool r=false;

  int32_t avail = self->m_cur_length - self->m_cur_read_pos;
  if( avail > 0 )
  {
    if( max_data_length > avail )
      max_data_length = avail;
    memcpy(data, &self->m_buffer[ self->m_cur_read_pos ], max_data_length );
    self->m_cur_read_pos += max_data_length;
    *data_length = max_data_length;
    r=true;
  }
  return r;
}


bool us_buffer_append_rounded_data(
        us_buffer_t *self,
        const void *data,
        int32_t data_length
        )
{
  bool r = true;
  int nul_count = 0;
  int i;
  r &= self->append(self, data, data_length);

  // length -> padding
  // 4 -> 0
  // 5 -> 3
  // 6 -> 2
  // 7 -> 1
  // 8 -> 0

  nul_count = (4 - (data_length & 3))&3;

  for (i = 0; i < nul_count && r == true; ++i)
  {
    r &= us_buffer_append_byte(self, 0);
  }

  return r;
}

bool us_buffer_read_rounded_data(
                                     us_buffer_t *self,
                                     void *data,
                                     int32_t max_data_length,
                                     int32_t *data_length
                                     )
{
  bool r=false;

  int32_t avail = self->m_cur_length - self->m_cur_read_pos;
  if( avail > 0 )
  {
    if( max_data_length > avail )
      max_data_length = avail;
    max_data_length = us_round_size( max_data_length );
    memcpy(data, &self->m_buffer[ self->m_cur_read_pos ], max_data_length );
    self->m_cur_read_pos += max_data_length;
    *data_length = max_data_length;

    r=true;
  }
  return r;
}


us_buffer_t *
us_buffer_init(
        us_buffer_t *self,
        us_allocator_t *allocator,
        void *raw_memory,
        int32_t raw_memory_length
        )
{
  us_buffer_t *r = 0;

  if (raw_memory)
  {
    self->destroy = us_buffer_destroy;
    self->append = us_buffer_append;
#if US_ENABLE_PRINTING
    self->print = us_buffer_print;
#endif
    self->m_allocator = allocator;
    self->m_buffer = (uint8_t *) raw_memory;
    self->m_cur_write_ptr = self->m_buffer;
    self->m_cur_read_pos = 0;
    self->m_max_length = raw_memory_length;
    self->m_cur_length = 0;

    r = self;
  }

  return r;
}

void us_buffer_reset(
                         us_buffer_t *self
                         )
{
  if( self )
  {
    self->m_cur_read_pos = 0;
    self->m_cur_write_ptr = self->m_buffer;
    self->m_cur_length = 0;
  }
}


us_buffer_t *
us_buffer_create(
        us_allocator_t *allocator,
        int32_t max_length
        )
{
  us_buffer_t *r = 0;
  us_buffer_t *self;

  self = us_new(allocator, us_buffer_t );

  if (self)
  {
    r = us_buffer_init(
            self,
            allocator,
            allocator->alloc(allocator, max_length, 1),
            max_length
            );
  }

  return r;
}

void
us_buffer_destroy(
        us_buffer_t *self
        )
{
  if( self->m_allocator )
    self->m_allocator->free( self->m_allocator, self->m_buffer);
}

bool
us_buffer_append(
        us_buffer_t *self,
        const void *data,
        int32_t data_length
        )
{
  bool r = false;

  int32_t new_length = self->m_cur_length + data_length;

  if (new_length <= self->m_max_length)
  {
    memcpy(self->m_cur_write_ptr, data, data_length);
    self->m_cur_write_ptr += data_length;
    self->m_cur_length += data_length;
    r = true;
  }

  return r;
}


