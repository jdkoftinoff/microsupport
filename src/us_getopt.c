#include "us_world.h"

#include "us_getopt.h"
#include "us_parse.h"

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

const char *us_getopt_value_types[] =
{
    "none",
    "flag",
    "char",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "hex16",
    "hex32",
    "string"
#if US_ENABLE_FLOAT
    ,
    "float"
#endif
};

bool us_getopt_string_for_value(
    char *buf,
    int buf_len,
    us_getopt_type_t type,
    const void *value
)
{
    bool r=false;
    if( value )
    {
        r=true;
        switch( type )
        {
        case US_GETOPT_NONE:
        {
            *buf='\0';
            break;
        }
        case US_GETOPT_FLAG:
        {
            *buf='\0';
            break;
        }
        case US_GETOPT_CHAR:
        {
            if( buf_len>2 )
            {
                buf[0]=*(char *)value;
                buf[1]='\0';
            }
            else
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_INT16:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "%hd", *(int16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT16:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "%hu", *(uint16_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_INT32:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "%d", *(int32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT32:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "%u", *(uint32_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX16:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%04hx", *(uint16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX32:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%08x", *(uint32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
#if US_ENABLE_FLOAT
        case US_GETOPT_FLOAT:
        {
            if( US_DEFAULT_SNPRINTF( buf, buf_len, "%f", *(float *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
#endif
        case US_GETOPT_STRING:
        {
            char tmp[1024] = "";
            *buf='\0';
            r=false;
//            if( us_getopt_escape( tmp, sizeof(tmp)-1, (const char *)value, strlen((const char *)value ) ) )
            {
                strcpy( tmp, (const char *)value );
                if( strlen( tmp )< (size_t)(buf_len-1) )
                {
                    strncpy( buf, tmp, buf_len-1 );
                    r=true;
                }
            }
            break;
        }
        default:
        {
            *buf='\0';
            break;
        }
        }
    }
    return r;
}

bool us_getopt_copy_value( void *value, us_getopt_type_t type, const void *default_value )
{
    bool r=true;
    if( default_value )
    {
        switch( type )
        {
        case US_GETOPT_NONE:
            break;
        case US_GETOPT_FLAG:
            *(bool *)value = *(bool *)default_value;
            break;
        case US_GETOPT_CHAR:
            *(char *)value = *(char *)default_value;
            break;
        case US_GETOPT_INT16:
            *(int16_t *)value = *(int16_t *)default_value;
            break;
        case US_GETOPT_UINT16:
            *(uint16_t *)value = *(uint16_t *)default_value;
            break;
        case US_GETOPT_INT32:
            *(int32_t *)value = *(int32_t *)default_value;
            break;
        case US_GETOPT_UINT32:
            *(uint32_t *)value = *(uint32_t *)default_value;
            break;
        case US_GETOPT_HEX16:
            *(uint16_t *)value = *(uint16_t *)default_value;
            break;
        case US_GETOPT_HEX32:
            *(uint32_t *)value = *(uint32_t *)default_value;
            break;
#ifdef US_ENABLE_FLOAT
        case US_GETOPT_FLOAT:
            *(float *)value = *(float *)default_value;
            break;
#endif
        case US_GETOPT_STRING:
            *(char **)value = strdup( (const char *)default_value );
            break;
        default:
            break;
        }
    }
    return r;
}


bool us_getopt_escape(char *dest, int dest_len, const char *str, int str_len )
{
    /* TODO: escape individual chars */
    bool r=false;
    if( dest_len>str_len+1 )
    {
        memcpy( dest, str, str_len+1 );
        dest[str_len+1] = '\0';
        r=true;
    }
    return r;
}

int us_getopt_unescape_char( char *dest, const char *str, int str_len )
{
    const char *p;
    int r=-1;
    if( str_len>0 )
    {
        p=str;
        if( *p=='\\' )
        {
            if( str_len>1 )
            {
                p++;
                switch( *p )
                {
                case '\\':
                    *dest='\\';
                    r=2;
                    break;
                case 't':
                    *dest='\t';
                    r=2;
                    break;
                case 'n':
                    *dest='\n';
                    r=2;
                    break;
                case 'r':
                    *dest='\r';
                    r=2;
                    break;
                case '\'':
                    *dest='\'';
                    r=2;
                    break;
                case '\"':
                    *dest='\"';
                    r=2;
                case 'x':
                    if( str_len>3 )
                    {
                        if( us_parse_hexoctet( (uint8_t *)dest, &p[1], 2, 0 ) )
                        {
                            r=4;
                        }
                    }
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                    if( str_len>3 )
                    {
                        if( isdigit( p[1]) && isdigit(p[2]))
                        {
                            *dest = ((p[0]-'0')<<6) + ((p[1]-'0')<<3) + ((p[2])-'0');
                            r=4;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            *dest=*p;
            r=1;
        }
    }
    return r;
}

bool us_getopt_unescape( char *dest, int dest_len, const char *str, int str_len )
{
    /* TODO: unescape individual chars */
    bool r=false;
    if( dest_len>str_len+1 )
    {
        memcpy( dest, str, str_len+1 );
        dest[str_len+1] = '\0';
        r=true;
    }
    return r;
}

bool us_getopt_value_for_string(
    us_getopt_type_t type,
    void *value,
    const char *str,
    int str_len
)
{
    bool r=true;
    /* TODO: use str_len */
    (void)str_len;
    switch( type )
    {
    case US_GETOPT_NONE:
        break;
    case US_GETOPT_FLAG:
        *(bool *)value = true;
        break;
    case US_GETOPT_CHAR:
        *(char *)value = str[0];
        break;
    case US_GETOPT_INT16:
        *(int16_t *)value = (int16_t)strtol( str, 0, 10 );
        break;
    case US_GETOPT_UINT16:
        *(uint16_t *)value = (uint16_t)strtoul( str, 0, 10 );
        break;
    case US_GETOPT_INT32:
        *(int32_t *)value = (int32_t)strtol( str, 0, 10 );
        break;
    case US_GETOPT_UINT32:
        *(uint32_t *)value = (uint32_t)strtoul( str, 0, 10 );
        break;
    case US_GETOPT_HEX16:
        *(uint16_t *)value = (uint16_t)strtoul( str, 0, 16 );
        break;
    case US_GETOPT_HEX32:
        *(uint32_t *)value = (uint32_t)strtoul( str, 0, 16 );
        break;
#ifdef US_ENABLE_FLOAT
    case US_GETOPT_FLOAT:
        *(float *)value = (float)atof( str );
        break;
#endif
    case US_GETOPT_STRING:
        *(char **)value = strdup( str );
        break;
    default:
        break;
    }
    return r;
}


bool us_getopt_init( us_getopt_t *self, us_allocator_t *allocator )
{
    self->destroy = us_getopt_destroy;
    self->m_allocator = allocator;
    self->m_option_lists = 0;
    self->m_last_option_list = 0;
    return true;
}

static void us_getopt_option_list_destroy( us_getopt_t *self, us_getopt_option_list_t *cur )
{
    if( cur->m_next )
    {
        us_getopt_option_list_destroy( self, cur->m_next );
        cur->m_next = 0;
    }
    us_delete( self->m_allocator, cur );
}

void us_getopt_destroy( us_getopt_t *self )
{
    us_getopt_option_list_t *cur = self->m_option_lists;
    if( cur )
    {
        us_getopt_option_list_destroy( self, cur );
    }
    self->m_option_lists = 0;
}

bool us_getopt_add_list( us_getopt_t *self, const us_getopt_option_t *options, const char *prefix, const char *description )
{
    bool r=false;
    us_getopt_option_list_t *p = 0;
    p = us_new( self->m_allocator, us_getopt_option_list_t );
    if( p )
    {
        p->m_options = options;
        p->m_next = 0;
        p->m_prefix = prefix;
        p->m_description = description;
        r=true;
        if( self->m_last_option_list )
        {
            self->m_last_option_list->m_next = p;
            self->m_last_option_list = p;
        }
        else
        {
            self->m_option_lists = p;
            self->m_last_option_list = p;
        }
    }
    return r;
}

bool us_getopt_fill_defaults( us_getopt_t *self )
{
    bool r=true;
    us_getopt_option_list_t *list = self->m_option_lists;
    while( list && r)
    {
        const us_getopt_option_t *opt = list->m_options;
        while( opt && r && opt->m_name!=0 )
        {
            us_getopt_copy_value( opt->m_current_value, opt->m_value_type, opt->m_default_value );
            opt++;
        }
        list=list->m_next;
    }
    return r;
}

bool us_getopt_print( us_getopt_t *self, us_print_t *printer )
{
    bool r=true;
    us_getopt_option_list_t *list = self->m_option_lists;
    while( list && r)
    {
        const us_getopt_option_t *opt = list->m_options;
        if( list->m_prefix )
        {
            r&=printer->printf( printer,"Option group '%s': %s\n", list->m_prefix, list->m_description );
        }
        while( opt && r && opt->m_name!=0 )
        {
            char default_string[1024] = "";
            /* TODO: Form default_string */
            if( !us_getopt_string_for_value( default_string, sizeof(default_string)-1, opt->m_value_type, opt->m_default_value ) )
            {
                default_string[0] = '\0';
            }
            r&=printer->printf( printer, "    %s%c%s (%s) : %s%s%s%s\n",
                                ( list->m_prefix ? list->m_prefix : "" ),
                                ( list->m_prefix ? '.' : ' ' ),
                                opt->m_name,
                                us_getopt_value_types[opt->m_value_type],
                                opt->m_description,
                                (( default_string[0]!='\0' ) ? ", default is: \"" : ""),
                                default_string,
                                (( default_string[0]!='\0' ) ? "\"" : "" )
                              );
            opt++;
        }
        list=list->m_next;
    }
    return r;
}

bool us_getopt_parse_one( us_getopt_t *self, const char *name, int name_len, const char *value, int value_len )
{
    bool r=false;
    us_getopt_option_list_t *list = self->m_option_lists;
    while( list && !r)
    {
        int prefix_len = 0;
        const char *subname = name;
        int subname_len=name_len;
        if( list->m_prefix!=0 )
        {
            prefix_len = strlen( list->m_prefix );
            subname+=(prefix_len+1);
            subname_len-=(prefix_len+1);
        }
        if( subname_len>0 && (prefix_len==0 || strncmp( name, list->m_prefix, prefix_len )==0 ) )
        {
            const us_getopt_option_t *opt = list->m_options;
            while( opt && opt->m_name!=0 )
            {
                if( strncmp( subname, opt->m_name, subname_len ) == 0 )
                {
                    r=us_getopt_value_for_string( opt->m_value_type, opt->m_current_value, value, value_len );
                    return r;
                }
                opt++;
            }
        }
        list = list->m_next;
    }
    return r;
}

bool us_getopt_parse_args( us_getopt_t *self, const char **argv )
{
    bool r=true;
    while( *argv )
    {
        if( (*argv)[0]=='-' && (*argv)[1]=='-' )
        {
            const char *pos_equals = strchr( *argv, '=' );
            const char *pos_name = (*argv)+2;
            int name_len = (pos_equals!=0 ? pos_equals-pos_name : (int)strlen(pos_name) );
            const char *pos_value = (pos_equals ? pos_equals+1 : 0 );
            int value_len = (pos_value ? strlen(pos_value) : 0 );
            r&=us_getopt_parse_one( self, pos_name, name_len, pos_value, value_len );
        }
        ++argv;
    }
    return r;
}

bool us_getopt_parse_file( us_getopt_t *self, const char *fname )
{
    /* TODO: */
    (void)self;
    (void)fname;
    return false;
}

bool us_getopt_parse_line( us_getopt_t *self, const char *line )
{
    /* TODO: */
    (void)self;
    (void)line;
    return false;
}

bool us_getopt_parse_buffer( us_getopt_t *self, us_buffer_t *buf )
{
    /* TODO: */
    (void)self;
    (void)buf;
    return false;
}
