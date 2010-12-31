#ifndef US_GETOPT_H
#define US_GETOPT_H

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
#include "us_buffer.h"
#include "us_print.h"

/**
 \addtogroup us_getopt program options handling
 */
/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        US_GETOPT_NONE=0,
        US_GETOPT_FLAG,
        US_GETOPT_CHAR,
        US_GETOPT_INT16,
        US_GETOPT_UINT16,
        US_GETOPT_INT32,
        US_GETOPT_UINT32,
        US_GETOPT_HEX16,
        US_GETOPT_HEX32,
        US_GETOPT_STRING,
#if US_ENABLE_FLOAT
        US_GETOPT_FLOAT
#endif
    }
    us_getopt_type_t;

    extern const char *us_getopt_value_types[];


    bool us_getopt_escape(char *dest, int dest_len, const char *str, int str_len );
    int us_getopt_unescape_char( char *dest, const char *str, int str_len );
    bool us_getopt_unescape( char *dest, int dest_len, const char *str, int str_len );

    bool us_getopt_string_for_value(
        char *buf,
        int buf_len,
        us_getopt_type_t type,
        const void *value
    );

    bool us_getopt_value_for_string(
        us_getopt_type_t type,
        void *value,
        const char *str,
        int str_len
    );

    typedef struct us_getopt_option_s
    {
        const char *m_name;
        const char *m_description;
        us_getopt_type_t m_value_type;
        const void * m_default_value;
        void * m_current_value;
    } us_getopt_option_t;

    typedef struct us_getopt_option_list_s
    {
        us_getopt_option_t *m_list;
        const char *m_prefix;
        const char *m_description;
        struct us_getopt_option_list_s *m_next;
    } us_getopt_option_list_t;

    typedef struct us_getopt_s
    {
        void (*destroy)( struct us_getopt_s *self );
        us_allocator_t *m_allocator;
        us_getopt_option_list_t *m_option_lists;
        us_getopt_option_list_t *m_last_option_list;
    } us_getopt_t;

    bool us_getopt_init( us_getopt_t *self, us_allocator_t *allocator );
    void us_getopt_destroy( us_getopt_t *self );
    bool us_getopt_add_list( us_getopt_t *self, const us_getopt_option_list_t list[], const char *prefix, const char *description );
    bool us_getopt_print( us_getopt_t *self, us_print_t *printer );
    bool us_getopt_parse_one( us_getopt_t *self, const char *name, const char *value, int value_len );
    bool us_getopt_parse_args( us_getopt_t *self, int argc, const char **argv );
    bool us_getopt_parse_file( us_getopt_t *self, const char *fname );
    bool us_getopt_parse_line( us_getopt_t *self, const char *line );
    bool us_getopt_parse_buffer( us_getopt_t *self, us_buffer_t *buf );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
