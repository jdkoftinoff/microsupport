#include "us_world.h"

#include "us_print.h"

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

#if US_ENABLE_PRINTING

us_print_t *us_stdout = 0;
us_print_t *us_stderr = 0;

#if US_ENABLE_STDIO

void us_print_file_destroy (
    us_print_t *self_
)
{
    us_print_file_t *self = ( us_print_file_t* ) self_;
    fflush ( self->m_f );
    fclose ( self->m_f );
}

bool us_print_file_printf (
    us_print_t *self_,
    const char *fmt,
    ...
)
{
    bool r = false;
    va_list ap;
    us_print_file_t *self = ( us_print_file_t* ) self_;
    va_start ( ap, fmt );
    if ( self && self->m_f )
    {
        if ( US_DEFAULT_VFPRINTF ( self->m_f, fmt, ap ) >= 0 )
        {
            r = true;
        }
    }
    va_end ( ap );
    return r;
}

bool us_print_file_vprintf (
    us_print_t *self_,
    const char *fmt,
    va_list ap
)
{
    bool r = false;
    us_print_file_t *self = ( us_print_file_t* ) self_;
    if ( self && self->m_f )
    {
        if ( US_DEFAULT_VFPRINTF ( self->m_f, fmt, ap ) >= 0 )
        {
            r = true;
        }
    }
    return r;
}


us_print_t *
us_print_file_init (
    us_print_file_t *self,
    FILE *f
)
{
    us_print_t *r = 0;
    if ( self && f )
    {
        self->m_f = f;
        self->m_base.destroy = us_print_file_destroy;
        self->m_base.printf = us_print_file_printf;
        self->m_base.vprintf = us_print_file_vprintf;
        r = &self->m_base;
    }
    return r;
}

#endif

us_print_t *
us_printraw_init (
    us_printraw_t *self,
    void *raw_memory,
    int raw_memory_length
)
{
    us_print_t *r = 0;
    if ( self && raw_memory )
    {
        self->m_buffer = ( char * ) raw_memory;
        self->m_max_length = raw_memory_length;
        self->m_cur_length = 0;
        self->m_base.destroy = us_printraw_destroy;
        self->m_base.printf = us_printraw_printf;
        self->m_base.vprintf = us_printraw_vprintf;
        r = &self->m_base;
    }
    return r;
}

void
us_printraw_destroy (
    us_print_t *self
)
{
    /* Do Nothing */
    ( void ) self;
}




bool us_printraw_printf (
    us_print_t *self_,
    const char *fmt,
    ...
)
{
    bool r = false;
    va_list ap;
    us_printraw_t *self = ( us_printraw_t* ) self_;
    va_start ( ap, fmt );
    if ( self )
    {
        char *cur_ptr = self->m_buffer + self->m_cur_length;
        int max_len = self->m_max_length - self->m_cur_length;
        if ( max_len > 0 )
        {
            int result = US_DEFAULT_VSNPRINTF ( cur_ptr, max_len, fmt, ap );
            if ( result < max_len )
            {
                self->m_cur_length += result;
                r = true;
            }
        }
    }
    va_end ( ap );
    return r;
}

bool us_printraw_vprintf (
    us_print_t *self_,
    const char *fmt,
    va_list ap
)
{
    bool r = false;
    us_printraw_t *self = ( us_printraw_t* ) self_;
    if ( self )
    {
        char *cur_ptr = self->m_buffer + self->m_cur_length;
        int max_len = self->m_max_length - self->m_cur_length;
        if ( max_len > 0 )
        {
            int result = US_DEFAULT_VSNPRINTF ( cur_ptr, max_len, fmt, ap );
            if ( result < max_len )
            {
                self->m_cur_length += result;
                r = true;
            }
        }
    }
    return r;
}


us_print_t *
us_printbuf_create (
    us_allocator_t *allocator,
    int memory_length
)
{
    us_print_t *r = 0;
    us_printbuf_t *self = us_new ( allocator, us_printbuf_t );
    if ( self )
    {
        self->m_base.m_buffer = (char *)allocator->alloc ( allocator, memory_length, 1 );
        self->m_base.m_base.destroy = us_printraw_destroy;
        self->m_base.m_base.printf = us_printraw_printf;
        self->m_base.m_base.vprintf = us_printraw_vprintf;
        if ( self->m_base.m_buffer )
        {
            r = &self->m_base.m_base;
        }
        else
        {
            us_delete ( allocator, self );
        }
    }
    return r;
}


#endif


