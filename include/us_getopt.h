#ifndef US_GETOPT_H
#define US_GETOPT_H

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
#include "us_buffer.h"
#include "us_print.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 \addtogroup us_getopt program options handling
 */
/*@{*/

typedef enum
{
    US_GETOPT_NONE = 0,
    US_GETOPT_FLAG,
    US_GETOPT_CHAR,
    US_GETOPT_INT16,
    US_GETOPT_UINT16,
    US_GETOPT_INT32,
    US_GETOPT_UINT32,
    US_GETOPT_HEX16,
    US_GETOPT_HEX32,
    US_GETOPT_HEX64,
    US_GETOPT_MACADDR,
    US_GETOPT_STRING
#if US_ENABLE_FLOAT
        ,
    US_GETOPT_FLOAT
#endif
} us_getopt_type_t;

extern const char *us_getopt_value_types[];

bool us_getopt_escape( char *dest, size_t dest_len, const char *str, size_t str_len );
int us_getopt_unescape_char( char *dest, const char *str, size_t str_len );
bool us_getopt_unescape( char *dest, size_t dest_len, const char *str, size_t str_len );

bool us_getopt_string_for_value( char *buf, int buf_len, us_getopt_type_t type, const void *value );

bool us_getopt_value_for_string( us_allocator_t *allocator, us_getopt_type_t type, void *value, const char *str, int str_len );

bool us_getopt_string_for_default( char *buf, int buf_len, us_getopt_type_t type, const void *value );

bool us_getopt_copy_value( us_allocator_t *allocator, void *value, us_getopt_type_t type, const void *default_value );

typedef struct us_getopt_option_s
{
    const char *m_name;
    const char *m_description;
    us_getopt_type_t m_value_type;
    const void *m_default_value;
    void *m_current_value;
} us_getopt_option_t;

typedef struct us_getopt_option_list_s
{
    const us_getopt_option_t *m_options;
    const char *m_prefix;
    const char *m_description;
    struct us_getopt_option_list_s *m_next;
} us_getopt_option_list_t;

typedef struct us_getopt_s
{
    void ( *destroy )( struct us_getopt_s *self );
    us_allocator_t *m_allocator;
    us_getopt_option_list_t *m_option_lists;
    us_getopt_option_list_t *m_last_option_list;
} us_getopt_t;

bool us_getopt_init( us_getopt_t *self, us_allocator_t *allocator );
void us_getopt_destroy( us_getopt_t *self );
bool us_getopt_fill_defaults( us_getopt_t *self );
bool us_getopt_add_list( us_getopt_t *self, const us_getopt_option_t *options, const char *prefix, const char *description );
bool us_getopt_dump( us_getopt_t *self, us_print_t *printer, const char *ignore_key );
bool us_getopt_print( us_getopt_t *self, us_print_t *printer );
bool us_getopt_parse_one( us_getopt_t *self, const char *name, int name_len, const char *value, int value_len );
bool us_getopt_parse_args( us_getopt_t *self, const char **argv );
bool us_getopt_parse_file( us_getopt_t *self, const char *fname );
bool us_getopt_parse_line( us_getopt_t *self, const char *line, size_t line_len );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
