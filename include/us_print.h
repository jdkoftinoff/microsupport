#ifndef US_PRINT_H
#define US_PRINT_H

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
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     \addtogroup us_print  Print Buffers
     */
    /*@{*/


    /** us_print_t

     interface for printing functions; can be used by implementations
     to direct printf's to files, streams or memory buffers.
     */
    typedef struct us_print_s
    {
        /** destroy

           Function pointer to destroy the print object.

           @param self ptr to object
           @returns void
         */
        void ( *destroy ) (
            struct us_print_s *self
        );

        /** printf

           printf implementation for the selected print object.

           @param self pointer to print object
           @param fmt standard sprintf formatting string
           @returns bool true on success, false on memory of file error
         */
        bool ( *printf ) (
            struct us_print_s *self,
            const char *fmt,
            ...
        );

        /** vprintf

         vprintf implementation for the selected print object.

         @param self pointer to print object
         @param fmt standard sprintf formatting string
         @param ap reference to va_list of parameters
         @returns bool true on success, false on memory of file error
         */
        bool ( *vprintf ) (
            struct us_print_s *self,
            const char *fmt,
            va_list ap
        );

    } us_print_t;

#if US_ENABLE_PRINTING
    extern us_print_t *us_stdout;
    extern us_print_t *us_stderr;

#if US_ENABLE_STDIO

    /** us_print_file_t

     printer which prints to a stdio FILE
     */

    typedef struct us_print_file_s
    {
        us_print_t m_base;
        FILE *m_f;
    } us_print_file_t;

    /**
     initialize a us_print_file_t structure, attach it
     to a stdio FILE *.

     @param self us_print_file_t structure to initialize
     @param f FILE to attach to
     @return self
     */

    us_print_t *
    us_print_file_init (
        us_print_file_t *self,
        FILE *f
    );

    void us_print_file_destroy (
        us_print_t *self_
    );

    bool us_print_file_printf (
        us_print_t *self_,
        const char *fmt,
        ...
    );

    bool us_print_file_vprintf (
        us_print_t *self_,
        const char *fmt,
        va_list ap
    );

    extern us_print_t *us_stdout;
    extern us_print_t *us_stderr;

#endif


    /**
     */
    typedef struct us_printraw_s
    {
        us_print_t m_base;
        char *m_buffer;
        int m_max_length;
        int m_cur_length;
    } us_printraw_t;

    us_print_t *
    us_printraw_init (
        us_printraw_t *self,
        void *raw_memory,
        int raw_memory_length
    );

    void
    us_printraw_destroy (
        us_print_t *self
    );

    bool
    us_printraw_printf (
        us_print_t *self,
        const char *fmt,
        ...
    );

    bool
    us_printraw_vprintf (
        us_print_t *self,
        const char *fmt,
        va_list ap
    );

    static inline const char *
    us_printraw_get (
        us_print_t *self_
    )
    {
        us_printraw_t *self = ( us_printraw_t * ) self_;
        return ( const char * ) self->m_buffer;
    }

    static inline size_t
    us_printraw_length (
        us_print_t *self_
    )
    {
        us_printraw_t *self = ( us_printraw_t * ) self_;
        return self->m_cur_length;
    }


    typedef struct us_printbuf_s
    {
        us_printraw_t m_base;
    } us_printbuf_t;

    us_print_t *
    us_printbuf_create (
        us_allocator_t *allocator,
        int memory_length
    );

    static inline const char *
    us_printbuf_get (
        us_print_t *self_
    )
    {
        us_printbuf_t *self = ( us_printbuf_t * ) self_;
        return self->m_base.m_buffer;
    }

    static inline size_t
    us_printbuf_length (
        us_print_t *self_
    )
    {
        us_printbuf_t *self = ( us_printbuf_t * ) self_;
        return self->m_base.m_cur_length;
    }

#endif


    /*@}*/
#ifdef __cplusplus
}
#endif

#endif
