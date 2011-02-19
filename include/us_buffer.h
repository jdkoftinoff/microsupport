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

        us_allocator_t *m_allocator;
        uint8_t *m_buffer;
        int32_t m_next_in;
        int32_t m_next_out;
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

    /** us_buffer_readable_count
    @param self us_buffer_t
    @returns int length of data that can be read from queue
    */
    static inline int us_buffer_readable_count ( const us_buffer_t *self )
    {
        return ( self->m_next_in - self->m_next_out + self->m_max_length ) % self->m_max_length;
    }

    /** us_buffer_contig_readable_count
     @param self us_buffer_t
     @returns int length of data that can be read from queue contiguously.
     */
    static inline int us_buffer_contig_readable_count ( const us_buffer_t *self )
    {
        if ( self->m_next_in < self->m_next_out )
            return ( self->m_max_length - self->m_next_out ) % self->m_max_length;
        else
            return ( self->m_next_in - self->m_next_out ) % self->m_max_length;
    }

    static inline const uint8_t *us_buffer_contig_read_ptr ( const us_buffer_t *self )
    {
        return &self->m_buffer[ self->m_next_out ];
    }


    /** us_buffer_can_read_byte
     @param self us_buffer_t to use
     @returns bool true if there is one or more data bytes available
     */
    static inline bool us_buffer_can_read_byte ( const us_buffer_t *self )
    {
        return ( self->m_next_out != self->m_next_in );
    }

    /** us_buffer_read_byte
     @param self us_buffer_t to use
     @returns uint8_t next byte read from queue
     */
    static inline uint8_t us_buffer_read_byte ( us_buffer_t *self )
    {
        uint8_t r = self->m_buffer[ self->m_next_out ];
        self->m_next_out = ( self->m_next_out + 1 ) % self->m_max_length;
        return r;
    }


    /** us_buffer_read Read Data from queue
     @param self us_buffer_t to read from
     @param dest_data data pointer to write to
     @param dest_data_cnt count of data to transfer
     @returns void
     */
    static inline void
    us_buffer_read ( us_buffer_t *self, uint8_t *dest_data, int dest_data_cnt )
    {
        int i;
        for( i=0; i<dest_data_cnt; ++i )
        {
            dest_data[i] = us_buffer_read_byte( self );
        }
    }

    /** us_buffer_peek Peek at data in buffer
     @param self us_buffer_t to peek at
     @param offset uint32_t offset to peek at
     @returns uint8_t value at position in queue
     */
    static inline uint8_t us_buffer_peek ( const us_buffer_t *self, int offset )
    {
        return self->m_buffer[ ( self->m_next_out + offset ) % self->m_max_length ];
    }

    static inline int32_t us_buffer_in_position ( const us_buffer_t *self )
    {
        return self->m_next_in;
    }

    static inline int32_t us_buffer_out_position ( const us_buffer_t *self )
    {
        return self->m_next_out;
    }


    /** us_buffer_poke Poke data to buffer at position
     @param self us_buffer_t to poke at
     @param offset uint32_t offset to peek at
     @param uint8_t value to position in queue
     @returns void
     */
    static inline void us_buffer_poke ( us_buffer_t *self, int offset, uint8_t val )
    {
        self->m_buffer[ offset ] = val;
    }

    /** us_buffer_poke_uint32 Poke uint32_t data to buffer at position
     @param self us_buffer_t to poke at
     @param offset uint32_t offset to peek at
     @param uint32_t value to position in queue
     @returns void
     */
    static inline void us_buffer_poke_uint32 ( us_buffer_t *self, int offset, uint32_t val )
    {
        self->m_buffer[ offset ] = US_GET_BYTE_3(val);
        self->m_buffer[ (offset+1) % self->m_max_length ] = US_GET_BYTE_2(val);
        self->m_buffer[ (offset+2) % self->m_max_length ] = US_GET_BYTE_1(val);
        self->m_buffer[ (offset+3) % self->m_max_length ] = US_GET_BYTE_0(val);
    }


    /** us_buffer_poke_int32 Poke int32_t data to buffer at position
     @param self us_buffer_t to poke at
     @param offset uint32_t offset to peek at
     @param int32_t value to position in queue
     @returns void
     */
    static inline void us_buffer_poke_int32 ( us_buffer_t *self, int offset, int32_t val )
    {
        self->m_buffer[ offset ] = US_GET_BYTE_3(val);
        self->m_buffer[ (offset+1) % self->m_max_length ] = US_GET_BYTE_2(val);
        self->m_buffer[ (offset+2) % self->m_max_length ] = US_GET_BYTE_1(val);
        self->m_buffer[ (offset+3) % self->m_max_length ] = US_GET_BYTE_0(val);
    }

    /** us_buffer_skip Skip data in buffer
     @param self us_buffer_t to modify
     @param count int number of bytes to skip
     */
    static inline void us_buffer_skip ( us_buffer_t *self, int count )
    {
        self->m_next_out = ( self->m_next_out + count ) % self->m_max_length;
    }

    /** us_buffer_writable_count
     @param self us_buffer_t to use
     @returns int length of data that can be written to queue
     */
    static inline int us_buffer_writable_count ( const us_buffer_t *self )
    {
        return ( ( self->m_next_out - self->m_next_in - 1  + self->m_max_length) % self->m_max_length );
    }

    static inline uint8_t *us_buffer_contig_write_ptr ( us_buffer_t *self )
    {
        return &self->m_buffer[ self->m_next_in ];
    }

    /** us_buffer_contig_writable_count
     @param self us_buffer_t
     @returns int length of data that can be written to queue contiguously.
     */
    static inline int us_buffer_contig_writable_count ( const us_buffer_t *self )
    {
        if ( self->m_next_out >= self->m_next_in )
            return ( ( self->m_max_length - self->m_next_in ) - 1  + self->m_max_length) % ( self->m_max_length );
        else
            return ( ( self->m_next_out - self->m_next_in ) - 1  + self->m_max_length) % ( self->m_max_length );
    }



    /** us_buffer_can_write_byte
     @param self us_buffer_t to use
     @returns bool true if there is space to write one byte into queue
     */
    static inline bool us_buffer_can_write_byte ( const us_buffer_t *self )
    {
        return (( self->m_next_in + 1) % self->m_max_length) != self->m_next_out;
    }

    /** us_buffer_write_byte
     @param self us_buffer_t to use
     @param value uint8_t to write
     @returns void
     */
    static inline void us_buffer_write_byte ( us_buffer_t *self, uint8_t value )
    {
        self->m_buffer[ self->m_next_in ] = value;
        self->m_next_in = ( self->m_next_in + 1) % ( self->m_max_length );
    }

    /** us_buffer_write Write Data to queue
     @param self us_buffer_t to write to
     @param src_data data pointer to read from
     @param src_data_cnt count of data to transfer
     @returns void
     */
    static inline void us_buffer_write ( us_buffer_t *self, const uint8_t *src_data, int src_data_cnt )
    {
        int i;
        for( i=0; i<src_data_cnt; ++i )
        {
            us_buffer_write_byte( self, src_data[i] );
        }
    }

    /** us_buffer_write_buffer
      @param self us_buffer_t to use
      @param buf contents of buf get written to self, buf is not cleared
      @returns void
      */
    static inline bool us_buffer_write_buffer( us_buffer_t *self, const us_buffer_t *buf )
    {
        bool r=false;
        int rc = us_buffer_readable_count(buf);
        if( us_buffer_writable_count(self) >= rc )
        {
            int i;
            /* todo: this can be replaced with 1 or two memcpy() calls */
            for( i=0; i<rc; ++i )
            {
                us_buffer_write_byte( self, us_buffer_peek(buf,i));
            }
            r=true;
        }
        return r;
    }



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
        if ( us_buffer_can_read_byte(self) )
        {
            p = ( const char * ) &self->m_buffer[ self->m_next_out ];
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
        if ( us_buffer_readable_count(self)>=count )
        {
            self->m_next_out = (self->m_next_out+count) % self->m_max_length;
            p = ( const char * ) &self->m_buffer[ self->m_next_out ];
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


    /** find the string with len len in the buffer at start_pos
      @param self the buffer to look in
      @param start_pos the position in the buffer to look in
      @param string the string to look for
      @param len the exact length to compare
      @return bool true if exact match is found
      */
    static inline bool
    us_buffer_string_compare( const us_buffer_t *self, int start_pos, const char *string, int len )
    {
        bool r=false;
        if( us_buffer_readable_count(self)>=len+start_pos )
        {
            int i;
            r=true;
            for( i=0; i<len; ++i )
            {
                if( us_buffer_peek( self, start_pos+i ) != (uint8_t)string[i] )
                {
                    r=false;
                    break;
                }
            }
        }
        return r;
    }

    /**

     Search the buffer from the current read position for character c,
     return -1 if it is not found by the end of buffer or end of line

     */
    int32_t
    us_buffer_find_string_len (
        const us_buffer_t *self,
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
    static inline bool
    us_buffer_append (
        us_buffer_t *self,
        const void *data_,
        int32_t data_length
    )
    {
        bool r=false;
        const uint8_t *data = (const uint8_t*)data_;
        if( us_buffer_writable_count(self)>=data_length )
        {
            us_buffer_write( self, data, data_length );
            r=true;
        }
        return r;
    }

    /**
     Append a single data byte to the buffer.

     @param self buffer to use
     @param value 8 bit value to append
     @returns bool true on success
     */
    static inline bool us_buffer_append_byte (
        us_buffer_t *self,
        int value
    )
    {
        bool r=false;
        if( us_buffer_can_write_byte(self))
        {
            uint8_t byte_value = ( uint8_t ) value;
            us_buffer_write_byte( self, byte_value );
            r=true;
        }
        return r;
    }


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
