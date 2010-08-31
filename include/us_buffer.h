#ifndef US_BUFFER_H
#define US_BUFFER_H

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
    * Neither the name of the Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"

#include "us_allocator.h"
#include "us_print.h"

/**
 \addtogroup us_buffer Memory Buffer
 */
/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif

    /** us_buffer_s

     interface for buffering functions; can be used by implementations
     to direct buffers to files, streams or memory buffers.
     */
    typedef struct us_buffer_s
    {
        /** destroy

         Function pointer to destroy the buffer object.

         @param self ptr to object
         @returns void
         */
        void ( *destroy ) (
            struct us_buffer_s * self
        );

        /** append

         append implementation for the selected buffer object.

         @param self pointer to buffer object
         @returns bool true on success, false on memory of file error
         */
        bool ( *append ) (
            struct us_buffer_s *self,
            const void *data,
            int32_t data_length
        );

#if US_ENABLE_PRINTING
        /** print

         print the raw hex data content of this buffer to the printer object.

         @param self pointer to buffer object
         @param printer pointer to printer object to print to.
         @returns bool true on sucess
         */
        bool ( *print ) (
            struct us_buffer_s *self,
            us_print_t * printer
        );
#endif
        us_allocator_t *m_allocator;
        uint8_t *m_buffer;
        uint8_t *m_cur_write_ptr;
        int32_t m_cur_read_pos;
        int32_t m_cur_length;
        int32_t m_max_length;

    } us_buffer_t;


    /**
     initialize a buffer object to use a raw memory block

     @param self buffer object to initialize
     @param allocator The memory allocator to use
     @param raw_memory raw memory pointer
     @param raw_memory_length length of raw memory
     @returns buffer object pointer upon success, 0 on error
     */
    us_buffer_t *
    us_buffer_init (
        us_buffer_t *self,
        us_allocator_t *allocator,
        void *raw_memory,
        int32_t raw_memory_length
    );

    /**
     create a buffer object with the memory block from an allocator

     @param allocator allocator to allocate memory from
     @param max_length length of memory to allocate
     @returns buffer pointer or 0 on failure
     */
    us_buffer_t *
    us_buffer_create (
        us_allocator_t *allocator,
        int32_t max_length
    );

    /**
     reset a buffers position, effectively starting the buffers
     position at the beginning

     @param self buffer pointer
     */
    void us_buffer_reset (
        us_buffer_t *self
    );

    /**
     destroy the buffer.

     @param self buffer to destroy
     */
    void
    us_buffer_destroy (
        us_buffer_t *self
    );


    /**
     Get the current read position from the buffer

     @param self buffer to use
     */
    static inline const char *
    us_buffer_read_ptr (
        us_buffer_t *self
    )
    {
        const char *p = 0;
        if ( self->m_cur_read_pos < self->m_cur_length )
        {
            p = ( const char * ) &self->m_buffer[ self->m_cur_read_pos ];
        }
        return p;
    }

    /**
     Advance the current read pointer by count bytes, return the new read pointer
     or null if unable to advance
     */
    static inline const char *
    us_buffer_advance (
        us_buffer_t *self,
        int32_t count
    )
    {
        const char *p = 0;
        if ( self->m_cur_read_pos + count < self->m_cur_length )
        {
            self->m_cur_read_pos += count;
            p = ( const char * ) &self->m_buffer[ self->m_cur_read_pos ];
        }
        return p;
    }

    /**
     Advance the current read pointer until any char specified in the delim_chars is found
     */
    bool
    us_buffer_skip_to_delim (
        us_buffer_t *self,
        const char *delim_chars
    );

    /**
     Advance the current read pointer until any char not specified in the delim_chars is found
     */

    bool
    us_buffer_skip_delim (
        us_buffer_t *self,
        const char *delim_chars
    );


    /**

     Search the buffer from the current read position for character c,
     return -1 if it is not found by the end of buffer or end of line

     */
    int32_t
    us_buffer_find_string_len (
        us_buffer_t *self,
        char search_char,
        char eol_char
    );

    /**
     Append data bytes to the buffer.

     @param self buffer to use
     @param data data pointer
     @param data_length length of data
     @returns bool true on success
     */
    bool
    us_buffer_append (
        us_buffer_t *self,
        const void *data,
        int32_t data_length
    );

    /**
     Append a single data byte to the buffer.

     @param self buffer to use
     @param value 8 bit value to append
     @returns bool true on success
     */
    bool
    us_buffer_append_byte (
        us_buffer_t *self,
        int value
    );


    /**
     Read a single byte from the buffer's current read position
     into value_ptr and increment the read position

     @param self buffer to read from
     @param value_ptr ptr to value to store byte into
     @returns bool true on success
     */
    bool
    us_buffer_read_byte (
        us_buffer_t *self,
        void *value_ptr
    );

    /**
     Append a null terminated ascii or utf-8 string to the buffer.

     @param self buffer to use
     @param str string to append
     @returns bool true on success
     */
    bool
    us_buffer_append_string (
        us_buffer_t *self,
        const char *str
    );


    /**
     Read a null terminated ascii or utf-8 string from the buffer at the current position
     into the string storage area pointed to by 'value', which has a maximum length of results_max_len.

     @param self buffer to read
     @param value string storage area
     @param result_max_len maximum length of value buffer
     @returns bool true if the string fit in the storage area.
     */
    bool
    us_buffer_read_string (
        us_buffer_t *self,
        char *value,
        int32_t result_max_len
    );

    bool
    us_buffer_read_line (
        us_buffer_t *self,
        char *value,
        int32_t result_max_len
    );



    /**
     Append a null terminated ascii or utf-8 string to the buffer at the current position.
     If the string including null termination is a multiple of 4 bytes, then this
     function acts identically to the normal append_string.  If the string including
     null terimination is not a multiple of 4 bytes, then extra zero padding is appended
     to make sure that the next data item would be aligned to a 32 bit word boundary

     @param self buffer to append to
     @param str string to append
     @returns bool true if string fit into buffer
    */
    bool
    us_buffer_append_rounded_string (
        us_buffer_t *self,
        const char *str
    );

    bool
    us_buffer_read_rounded_string (
        us_buffer_t *self,
        char *value,
        int32_t result_max_len
    );

    bool
    us_buffer_append_int32 (
        us_buffer_t *self,
        int32_t value
    );

    bool
    us_buffer_read_int32 (
        us_buffer_t *self,
        int32_t *value_ptr
    );

    bool
    us_buffer_append_uint64 (
        us_buffer_t *self,
        uint32_t value_high,
        uint32_t value_low
    );

    bool
    us_buffer_read_uint64 (
        us_buffer_t *self,
        uint32_t *value_high_ptr,
        uint32_t *value_low_ptr
    );

#ifdef US_ENABLE_FLOAT
    bool
    us_buffer_append_float32 (
        us_buffer_t *self,
        float value
    );

    bool
    us_buffer_read_float32 (
        us_buffer_t *self,
        float *value_ptr
    );

#endif

#ifdef US_ENABLE_DOUBLE
    bool
    us_buffer_append_float64 (
        us_buffer_t *self,
        double value
    );
    bool
    us_buffer_read_float64 (
        us_buffer_t *self,
        double *value_ptr
    );
#endif

    bool
    us_buffer_append_data (
        us_buffer_t *self,
        const void *data,
        int32_t data_length
    );

    bool
    us_buffer_read_data (
        us_buffer_t *self,
        void *data,
        int32_t max_data_length,
        int32_t *data_length
    );

    bool
    us_buffer_append_rounded_data (
        us_buffer_t *self,
        const void *data,
        int32_t data_length
    );

    bool
    us_buffer_read_rounded_data (
        us_buffer_t *self,
        void *data,
        int32_t max_data_length,
        int32_t *data_length
    );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
