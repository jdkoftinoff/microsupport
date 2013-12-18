#ifndef US_QUEUE_H
#define US_QUEUE_H

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
    * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"
#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup queue
 */
/*@{*/

typedef struct us_queue_s {
    int m_next_in;
    int m_next_out;
    int m_buf_size;
    uint8_t *m_buf;
} us_queue_t;

/** us_queue_init Initialize Queue. Does no memory allocation.

 @param self us_queue_t to initialize
 @param buf pointer to raw data
 @param buf_size size of buffer, Buffer must be power of two bytes long
 */
void us_queue_init(us_queue_t *self, uint8_t *buf, int buf_size);

/** us_queue_readable_count
 @param self us_queue_t
 @returns int length of data that can be read from queue
 */
static inline int us_queue_readable_count(us_queue_t *self) {
    return (self->m_next_in - self->m_next_out) & (self->m_buf_size - 1);
}

/** us_queue_contig_readable_count
 @param self us_queue_t
 @returns int length of data that can be read from queue contiguously.
 */
static inline int us_queue_contig_readable_count(us_queue_t *self) {
    if (self->m_next_in < self->m_next_out)
        return (self->m_buf_size - self->m_next_out) & (self->m_buf_size - 1);
    else
        return (self->m_next_in - self->m_next_out) & (self->m_buf_size - 1);
}

static inline uint8_t *us_queue_contig_read_ptr(us_queue_t *self) { return &self->m_buf[self->m_next_out]; }

/** us_queue_read Read Data from queue
 @param self us_queue_t to read from
 @param dest_data data pointer to write to
 @param dest_data_cnt count of data to transfer
 @returns void
 */
void us_queue_read(us_queue_t *self, uint8_t *dest_data, int dest_data_cnt);

/** us_queue_can_read_byte
 @param self us_queue_t to use
 @returns bool true if there is one or more data bytes available
 */
static inline bool us_queue_can_read_byte(us_queue_t *self) { return (self->m_next_out != self->m_next_in); }

/** us_queue_read_byte
 @param self us_queue_t to use
 @returns uint8_t next byte read from queue
 */
static inline uint8_t us_queue_read_byte(us_queue_t *self) {
    uint8_t r = self->m_buf[self->m_next_out];
    self->m_next_out = (self->m_next_out + 1) & (self->m_buf_size - 1);
    return r;
}

/** us_queue_peek Peek at data in buffer
 @param self us_queue_t to peek at
 @param offset uint1_t offset to peek at
 @returns uint8_t value at position in queue
 */
static inline uint8_t us_queue_peek(us_queue_t *self, int offset) {
    return self->m_buf[(self->m_next_out + offset) & (self->m_buf_size - 1)];
}

/** us_queue_skip Skip data in buffer
 @param self us_queue_t to modify
 @param count int number of bytes to skip
 */
static inline void us_queue_skip(us_queue_t *self, int count) {
    self->m_next_out = (self->m_next_out + count) & (self->m_buf_size - 1);
}

/** us_queue_writable_count
 @param self us_queue_t to use
 @returns int length of data that can be written to queue
 */
static inline int us_queue_writable_count(us_queue_t *self) {
    int mask = self->m_buf_size - 1;
    return ((self->m_next_out - self->m_next_in - 1) & mask);
}

static inline uint8_t *us_queue_contig_write_ptr(us_queue_t *self) { return &self->m_buf[self->m_next_in]; }

/** us_queue_contig_writable_count
 @param self us_queue_t
 @returns int length of data that can be written to queue contiguously.
 */
static inline int us_queue_contig_writable_count(us_queue_t *self) {
    if (self->m_next_out >= self->m_next_in)
        return ((self->m_buf_size - self->m_next_in) - 1) & (self->m_buf_size - 1);
    else
        return ((self->m_next_out - self->m_next_in) - 1) & (self->m_buf_size - 1);
}

/** us_queue_write Write Data to queue
 @param self us_queue_t to write to
 @param src_data data pointer to read from
 @param src_data_cnt count of data to transfer
 @returns void
 */
void us_queue_write(us_queue_t *self, const uint8_t *src_data, int src_data_cnt);

/** us_queue_can_write_byte
 @param self us_queue_t to use
 @returns bool true if there is space to write one byte into queue
 */
static inline bool us_queue_can_write_byte(us_queue_t *self) {
    return ((self->m_next_out - self->m_next_in) & (self->m_buf_size - 1)) - 1 != 0;
}

/** us_queue_write_byte
 @param self us_queue_t to us
 @param value uint8_t to write
 @returns void
 */
static inline void us_queue_write_byte(us_queue_t *self, uint8_t value) {
    self->m_buf[self->m_next_in] = value;
    self->m_next_in = (self->m_next_in + 1) & (self->m_buf_size - 1);
}

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
