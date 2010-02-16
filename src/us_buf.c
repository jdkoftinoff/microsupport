#include "us_world.h"
#include "us_buf.h"

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
