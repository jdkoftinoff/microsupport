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

bool us_buffer_append_string (
    us_buffer_t *self,
    const char *str
)
{
    bool r = true;
    int32_t len = ( int32_t ) strlen ( str );
    r &= us_buffer_append ( self, str, len );
    return r;
}

bool us_buffer_read_string (
    us_buffer_t *self,
    char *value_ptr,
    size_t result_max_len
)
{
    bool r = false;
    size_t cur_length = us_buffer_readable_count(self);
    if ( cur_length>1 )
    {
        size_t i;
        int l=0;
        if ( cur_length>result_max_len-1 )
        {
            cur_length=result_max_len-1;
        }
        for ( i=0; i<cur_length; ++i )
        {
            value_ptr[i] = us_buffer_peek( self, i );
            l++;
            if ( value_ptr[i]=='\0' )
                break;
        }
        if ( l<=result_max_len )
        {
            us_buffer_skip( self, l );
            value_ptr[ result_max_len -1 ] = '\0';
            r=true;
        }
    }
    return r;
}

ssize_t
us_buffer_find_string_len (
    const us_buffer_t *self,
    char search_char,
    char eol_char
)
{
    ssize_t r = -1;
    size_t pos;
    int cur_length = us_buffer_readable_count(self);
    for ( pos = 0; pos < cur_length; ++pos )
    {
        if ( us_buffer_peek(self,pos) == search_char )
        {
            r = pos;
            break;
        }
        if ( us_buffer_peek(self,pos) == eol_char )
        {
            break;
        }
    }
    return r;
}

bool
us_buffer_read_line (
    us_buffer_t *self,
    char *value,
    size_t result_max_len
)
{
    bool r = false;
    size_t i = 0;
    size_t p = 0;
    size_t cur_length = us_buffer_readable_count(self);
    while ( i < result_max_len - 1 && p<cur_length )
    {
        char c = us_buffer_read_byte(self);
        p++;
        /* skip carriage returns */
        if ( c == '\r' )
            continue;
        /* all other characters get placed into value */
        value[i++] = c;
        /* eol means we are done */
        if ( c == '\n' )
        {
            value[i+1] = '\0';
            r = true;
            break;
        }
    }
    return r;
}

bool
us_buffer_scan_line (
    us_buffer_t *self,
    char *value,
    size_t result_max_len
)
{
    bool r = false;
    size_t i = 0;
    size_t p = 0;
    size_t cur_length = us_buffer_readable_count(self);
    while ( i < result_max_len - 1 && p<cur_length )
    {
        char c = us_buffer_peek(self,p++);
        /* skip carriage returns */
        if ( c == '\r' )
            continue;
        /* eol means we are done */
        if ( c == '\n' )
        {
            value[i] = '\0';
            r = true;
            /* skip to end of line */
            us_buffer_skip( self, p );
            break;
        }
        /* all other characters get placed into value */
        value[i++] = c;
    }
    return r;
}


bool
us_buffer_skip_to_delim (
    us_buffer_t *self,
    const char *delim_chars
)
{
    bool r = false;
    int delim_chars_len = ( int ) ( strlen ( delim_chars ) );
    while ( us_buffer_can_read_byte(self))
    {
        size_t i;
        char c=us_buffer_peek(self,0);
        for ( i = 0; i < delim_chars_len; ++i )
        {
            if ( c == delim_chars[i] )
            {
                r = true;
                break;
            }
        }
        if ( r == true )
        {
            break;
        }
        us_buffer_skip(self,1);
    }
    return r;
}

bool
us_buffer_skip_delim (
    us_buffer_t *self,
    const char *delim_chars
)
{
    bool r = false;
    size_t delim_chars_len = ( strlen ( delim_chars ) );
    while ( us_buffer_can_read_byte(self))
    {
        size_t i;
        char c=us_buffer_peek(self,0);
        bool skip = false;
        for ( i = 0; i < delim_chars_len; ++i )
        {
            if ( c == delim_chars[i] )
            {
                skip = true;
                break;
            }
        }
        if ( !skip )
        {
            r = true;
            break;
        }
        us_buffer_skip(self,1);
    }
    return r;
}

bool us_buffer_append_rounded_string (
    us_buffer_t *self,
    const char *str
)
{
    bool r = true;
    size_t nul_count = 0;
    size_t i;
    size_t len = ( int32_t ) strlen ( str );
    r &= us_buffer_append ( self, str, len );
    /*
    // length -> padding
    // 4 -> 4
    // 5 -> 3
    // 6 -> 2
    // 7 -> 1
    // 8 -> 4
    */
    nul_count = 4 - ( len & 3 );
    for ( i = 0; i < nul_count && r == true; ++i )
    {
        r &= us_buffer_append_byte ( self, 0 );
    }
    return r;
}

bool us_buffer_read_rounded_string (
    us_buffer_t *self,
    char *value,
    size_t result_max_len
)
{
    bool r = false;
    r = us_buffer_read_string (
            self,
            value,
            result_max_len
        );
    if ( r )
    {
        /* account for extra padding */
        size_t len = strlen(value);
        size_t skip = 3-(len&3);
        us_buffer_skip( self, skip );
    }
    return r;
}


bool us_buffer_append_int32 (
    us_buffer_t *self,
    int32_t value
)
{
    bool r = true;
    r &= us_buffer_append_byte ( self, US_GET_BYTE_3 ( value ) );
    r &= us_buffer_append_byte ( self, US_GET_BYTE_2 ( value ) );
    r &= us_buffer_append_byte ( self, US_GET_BYTE_1 ( value ) );
    r &= us_buffer_append_byte ( self, US_GET_BYTE_0 ( value ) );
    return r;
}


bool us_buffer_read_int32 (
    us_buffer_t *self,
    int32_t *value_ptr
)
{
    bool r = false;
    int32_t value = 0;
    if ( us_buffer_readable_count(self)>=4 )
    {
        value = ( ( int32_t ) us_buffer_read_byte(self)) << 24;
        value |= ( ( int32_t ) us_buffer_read_byte(self) ) << 16;
        value |= ( ( int32_t ) us_buffer_read_byte(self) ) << 8;
        value |= ( ( int32_t ) us_buffer_read_byte(self) ) << 0;
        *value_ptr = value;
        r = true;
    }
    return r;
}


bool us_buffer_append_uint64 (
    us_buffer_t *self,
    uint32_t value_high,
    uint32_t value_low
)
{
    bool r = true;
    r &= us_buffer_append_int32 (
             self,
             value_high
         );
    r &= us_buffer_append_int32 (
             self,
             value_low
         );
    return r;
}

bool us_buffer_read_uint64 (
    us_buffer_t *self,
    uint32_t *value_high_ptr,
    uint32_t *value_low_ptr
)
{
    bool r = true;
    r &= us_buffer_read_int32 ( self, ( int32_t * ) value_high_ptr );
    if ( r )
    {
        r &= us_buffer_read_int32 ( self, ( int32_t * ) value_low_ptr );
    }
    return r;
}

bool us_buffer_append_float32 (
    us_buffer_t *self,
    float value
)
{
    bool r = true;
    float *value_ptr = &value;
    int32_t int_value = * ( ( int32_t * ) value_ptr );
    r &= us_buffer_append_int32 (
             self,
             int_value
         );
    return r;
}


bool us_buffer_read_float32 (
    us_buffer_t *self,
    float *value_ptr
)
{
    bool r = true;
    r &= us_buffer_read_int32 ( self, ( int32_t * ) value_ptr );
    /* check for NaN and refuse to accept it */
    if ( *value_ptr != *value_ptr )
        r = false;
    return r;
}


#if US_ENABLE_DOUBLE

union us_float64_splitter_u
{
    double value;
    uint32_t ints[2];
};

bool us_buffer_append_float64 (
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
    r &= us_buffer_append_int32 (
             self,
             high_int_value
         );
    r &= us_buffer_append_int32 (
             self,
             low_int_value
         );
    return r;
}

bool us_buffer_read_float64 (
    us_buffer_t *self,
    double *value_ptr
)
{
    bool r = true;
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
    r &= us_buffer_read_uint64 ( self, high_int_value_ptr, low_int_value_ptr );
    if ( r )
    {
        *value_ptr = splitter.value;
        /* check for NaN and refuse to accept it */
        if ( *value_ptr != *value_ptr )
            r = false;
    }
    return r;
}

#endif

bool us_buffer_append_data (
    us_buffer_t *self,
    const void *data,
    int32_t data_length
)
{
    bool r = true;
    r &= us_buffer_append ( self, data, data_length );
    return r;
}

bool us_buffer_read_data (
    us_buffer_t *self,
    void *data_,
    int32_t max_data_length,
    int32_t *data_length
)
{
    bool r = false;
    int32_t avail = us_buffer_readable_count( self );
    uint8_t *data = (uint8_t *)data_;
    if ( avail > 0 )
    {
        int i;
        if ( max_data_length > avail )
            max_data_length = avail;
        for ( i=0; i<max_data_length; ++i )
        {
            data[i] = us_buffer_read_byte( self );
        }
        *data_length = max_data_length;
        r = true;
    }
    return r;
}


bool us_buffer_append_rounded_data (
    us_buffer_t *self,
    const void *data,
    int32_t data_length
)
{
    bool r = true;
    int nul_count = 0;
    int i;
    r &= us_buffer_append ( self, data, data_length );
    // length -> padding
    // 4 -> 0
    // 5 -> 3
    // 6 -> 2
    // 7 -> 1
    // 8 -> 0
    nul_count = ( 4 - ( data_length & 3 ) ) & 3;
    for ( i = 0; i < nul_count && r == true; ++i )
    {
        r &= us_buffer_append_byte ( self, 0 );
    }
    return r;
}

bool us_buffer_read_rounded_data (
    us_buffer_t *self,
    void *data_,
    int32_t max_data_length,
    int32_t *data_length
)
{
    bool r = false;
    int32_t avail = us_buffer_readable_count(self);
    if ( avail > 0 )
    {
        int i;
        uint8_t *data = (uint8_t *)data_;
        if ( max_data_length > avail )
            max_data_length = avail;
        max_data_length = us_round_size ( max_data_length );
        for ( i=0; i<max_data_length; ++i )
        {
            data[i] = us_buffer_read_byte(self);
        }
        *data_length = max_data_length;
        r = true;
    }
    return r;
}


us_buffer_t *
us_buffer_init (
    us_buffer_t *self,
    us_allocator_t *allocator,
    void *raw_memory,
    size_t raw_memory_length
)
{
    us_buffer_t *r = 0;
    if ( raw_memory )
    {
        self->destroy = us_buffer_destroy;
        self->m_allocator = allocator;
        self->m_buffer = ( uint8_t * ) raw_memory;
        self->m_next_in = 0;
        self->m_next_out = 0;
        self->m_max_length = raw_memory_length;
        r = self;
    }
    return r;
}

void us_buffer_reset (
    us_buffer_t *self
)
{
    if ( self )
    {
        self->m_next_in=0;
        self->m_next_out=0;
    }
}


us_buffer_t *
us_buffer_create (
    us_allocator_t *allocator,
    size_t max_length
)
{
    us_buffer_t *r = 0;
    us_buffer_t *self;
    self = us_new ( allocator, us_buffer_t );
    if ( self )
    {
        r = us_buffer_init (
                self,
                allocator,
                allocator->alloc ( allocator, max_length, 1 ),
                max_length
            );
    }
    return r;
}

void
us_buffer_destroy (
    us_buffer_t *self
)
{
    if ( self->m_allocator )
    {
        us_delete ( self->m_allocator, self->m_buffer );
        us_delete ( self->m_allocator, self );
    }
}

