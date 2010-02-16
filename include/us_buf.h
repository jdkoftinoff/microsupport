#ifndef US_BUF_H
#define US_BUF_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /** \addtogroup us_buf
  */
  /*@{*/

  typedef struct us_buf_s
  {
    int m_next_in;
    int m_next_out;
    int m_buf_size;
    uint8_t *m_buf;
  } us_buf_t;


  void us_buf_init(
                      us_buf_t *self,
                      uint8_t *buf,
                      int buf_size
                      );
  int us_buf_readable_count( us_buf_t *self );
  void us_buf_read( us_buf_t *self, uint8_t *dest_data, int dest_data_cnt );
  int us_buf_writeable_count( us_buf_t *self );
  void us_buf_write( us_buf_t *self, uint8_t *src_data, int src_data_cnt );

  /*@}*/

#ifdef __cplusplus
}
#endif


#endif
