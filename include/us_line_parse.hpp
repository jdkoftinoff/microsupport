#ifndef US_LINE_PARSE_HPP
#define US_LINE_PARSE_HPP

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

#include "us_world.hpp"

/**
 \addtogroup us_line_parse Direct from ascii line parsing
 */
/*@{*/

    static inline uint16_t us_line_parse_string_len ( void *extra )
    {
        const char *s = ( const char * ) extra;
        return strlen ( s );
    }

    static inline uint8_t us_line_parse_string_get ( void *extra, uint16_t pos )
    {
        uint8_t *s = ( uint8_t * ) extra;
        return s[pos];
    }



    typedef struct us_line_parse_s
    {
        uint16_t ( *len ) ( void *extra );
        uint8_t ( *get ) ( void *extra, uint16_t pos );
        void * m_extra;
        uint16_t m_scan_pos;
        int16_t m_next_delim_pos;
    } us_line_parse_t;

    static inline
    void us_line_parse_init (
        us_line_parse_t *self,
        void *extra,
        uint16_t ( *get_line_len ) ( void *extra ),
        uint8_t ( *get_character ) ( void *extra, uint16_t pos )
    )
    {
        self->m_extra = extra;
        self->len = get_line_len;
        self->get = get_character;
        self->m_scan_pos = 0;
        self->m_next_delim_pos = -1;
    }

    static inline
    bool us_line_parse_notify ( us_line_parse_t *self )
    {
        bool r = false;
        if ( self->m_next_delim_pos == -1 )
        {
            uint16_t pos = self->m_scan_pos;
            uint16_t len = self->len ( self->m_extra );
            for ( ; pos < len; ++pos )
            {
                uint8_t c = self->get ( self->m_extra, pos );
                if ( c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                {
                    self->m_next_delim_pos = pos;
                    r = true;
                    break;
                }
            }
        }
        return r;
    }

    static inline
    bool us_line_parse_extract_bool (
        us_line_parse_t *self,
        bool *value
    )
    {
        bool r = false;
        uint8_t c = self->get ( self->m_extra, self->m_scan_pos );
        if ( c == 'T' || c == 't' || c == '1' )
        {
            *value = true;
            r = true;
        }
        else if ( c == 'F' || c == 'f' || c == '0' )
        {
            r = true;
            *value = false;
        }
        return r;
    }

    bool us_line_parse_extract_int16 (
        us_line_parse_t *self,
        int16_t *value,
        int base
    );

    bool us_line_parse_extract_uint16 (
        us_line_parse_t *self,
        uint16_t *value,
        int base
    );

    bool us_line_parse_extract_int32 (
        us_line_parse_t *self,
        int32_t *value,
        int base
    );

    bool us_line_parse_extract_uint32 (
        us_line_parse_t *self,
        uint32_t *value,
        int base
    );

    bool us_line_parse_extract_int64 (
        us_line_parse_t *self,
        int64_t *value,
        int base
    );

    bool us_line_parse_extract_uint64 (
        us_line_parse_t *self,
        uint64_t *value,
        int base
    );

    bool us_line_parse_extract_string (
        us_line_parse_t *self,
        uint8_t *value,
        uint16_t max_len
    );



/*@}*/

#endif
