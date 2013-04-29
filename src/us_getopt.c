#include "us_world.h"

#include "us_getopt.h"
#include "us_parse.h"

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
    "hex64",
    "macaddr",
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
    if ( value )
    {
        r=true;
        switch ( type )
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
            if ( buf_len>2 )
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
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%hd", *(int16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT16:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%hu", *(uint16_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_INT32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%d", *(int32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%u", *(uint32_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX16:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%04hx", *(uint16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%08x", *(uint32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX64:
        {
            uint32_t high32 = (uint32_t)((*(uint64_t*)value)>>32);
            uint32_t low32 = (uint32_t)(*(uint64_t*)value)&0xffffffff;
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%08x%08x", high32, low32 ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_MACADDR:
        {
            uint8_t *macaddr = (uint8_t *)value;
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x",
                                      macaddr[0],
                                      macaddr[1],
                                      macaddr[2],
                                      macaddr[3],
                                      macaddr[4],
                                      macaddr[5]
                                    ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_STRING:
        {
            const char *str = *(const char **)value;
            char tmp[1024] = "";
            *buf='\0';
            r=false;
            if ( us_getopt_escape( tmp, sizeof(tmp)-1, str, strlen(str) ) )
            {
                if ( strlen( tmp )< (size_t)(buf_len-1) )
                {
                    strncpy( buf, tmp, buf_len-1 );
                    r=true;
                }
            }
            break;
        }
#if US_ENABLE_FLOAT
        case US_GETOPT_FLOAT:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%f", *(float *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
#endif
        default:
        {
            *buf='\0';
            break;
        }
        }
    }
    return r;
}

bool us_getopt_string_for_default(
    char *buf,
    int buf_len,
    us_getopt_type_t type,
    const void *value
)
{
    bool r=false;
    if ( value )
    {
        r=true;
        switch ( type )
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
            if ( buf_len>2 )
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
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%hd", *(int16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT16:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%hu", *(uint16_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_INT32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%d", *(int32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_UINT32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%u", *(uint32_t *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX16:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%04hx", *(uint16_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX32:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%08x", *(uint32_t*)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_HEX64:
        {
            uint32_t high32 = (uint32_t)((*(uint64_t*)value)>>32);
            uint32_t low32 = (uint32_t)(*(uint64_t*)value)&0xffffffff;
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "0x%08x%08x", high32, low32 ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_MACADDR:
        {
            uint8_t *macaddr = (uint8_t *)value;
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x",
                                      macaddr[0],
                                      macaddr[1],
                                      macaddr[2],
                                      macaddr[3],
                                      macaddr[4],
                                      macaddr[5]
                                    ) > buf_len )
            {
                r=false;
            }
            break;
        }
        case US_GETOPT_STRING:
        {
            const char *str = (const char *)value;
            char tmp[1024] = "";
            *buf='\0';
            r=false;
            if ( us_getopt_escape( tmp, sizeof(tmp)-1, str, strlen(str) ) )
            {
                if ( strlen( tmp )< (size_t)(buf_len-1) )
                {
                    strncpy( buf, tmp, buf_len-1 );
                    r=true;
                }
            }
            break;
        }
#if US_ENABLE_FLOAT
        case US_GETOPT_FLOAT:
        {
            if ( US_DEFAULT_SNPRINTF( buf, buf_len, "%f", *(float *)value ) > buf_len )
            {
                r=false;
            }
            break;
        }
#endif
        default:
        {
            *buf='\0';
            break;
        }
        }
    }
    return r;
}


bool us_getopt_copy_value( us_allocator_t *allocator, void *value, us_getopt_type_t type, const void *default_value )
{
    bool r=true;
    if ( default_value )
    {
        switch ( type )
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
        case US_GETOPT_HEX64:
            *(uint64_t *)value = *(uint64_t *)default_value;
            break;
        case US_GETOPT_MACADDR:
        {
            uint8_t *dest = (uint8_t *)value;
            const uint8_t *src = (uint8_t *)default_value;
            int i;
            for ( i=0; i<6; ++i )
            {
                dest[i] = src[i];
            }
            break;
        }
        break;
        case US_GETOPT_STRING:
            *(char **)value = us_strdup( allocator, (const char *)default_value );
            break;
#ifdef US_ENABLE_FLOAT
        case US_GETOPT_FLOAT:
            *(float *)value = *(float *)default_value;
            break;
#endif
        default:
            us_log_error( "Logic error: unknown getopt value type" );
            abort();
            break;
        }
    }
    return r;
}


bool us_getopt_escape(char *dest, size_t dest_len, const char *str, size_t str_len )
{
    bool r=true;
    size_t i;
    size_t dp=0;
    for (i=0; i<str_len; ++i)
    {
        char c=str[i];
        if ( dp>dest_len-4 )
        {
            r=false;
            break;
        }
        switch (c)
        {
        case '"':
            dest[dp++] = '\\';
            dest[dp++] = '"';
            break;
        case '\r':
            dest[dp++] = '\\';
            dest[dp++] = 'r';
            break;
        case '\n':
            dest[dp++] = '\\';
            dest[dp++] = 'n';
            break;
        case '\t':
            dest[dp++] = '\\';
            dest[dp++] = 't';
            break;
        default:
            if ( isprint(c) )
            {
                dest[dp++] = c;
            }
            else
            {
                US_DEFAULT_SNPRINTF( &dest[dp], 3, "\\%2x", c );
                dp+=3;
            }
            break;
        }
    }
    if ( r )
    {
        dest[dp] = '\0';
    }
    else
    {
        dest[0] = '\0';
    }
    return r;
}

int us_getopt_unescape_char( char *dest, const char *str, size_t str_len )
{
    const char *p;
    int r=-1;
    if ( str_len>0 )
    {
        p=str;
        if ( *p=='\\' )
        {
            if ( str_len>1 )
            {
                p++;
                switch ( *p )
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
                    if ( str_len>3 )
                    {
                        if ( us_parse_hexoctet( (uint8_t *)dest, &p[1], 2, 0 ) )
                        {
                            r=4;
                        }
                    }
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                    if ( str_len>3 )
                    {
                        if ( isdigit( p[1]) && isdigit(p[2]))
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

bool us_getopt_unescape( char *dest, size_t dest_len, const char *str, size_t str_len )
{
    bool r=true;
    size_t i;
    size_t dp=0;
    for ( i=0; i<str_len; )
    {
        int todo=(int32_t)(str_len-i);
        int done=0;
        if ( dp>dest_len-2 )
        {
            r=false;
            break;
        }
        done=us_getopt_unescape_char( &dest[dp], &str[i], todo );
        dp+=done;
        i+=done;
    }
    if ( r )
    {
        dest[dp] = '\0';
    }
    else
    {
        dest[0] = '\0';
    }
    return r;
}

bool us_getopt_value_for_string(
    us_allocator_t *allocator,
    us_getopt_type_t type,
    void *value,
    const char *str,
    int str_len
)
{
    bool r=true;
    char input_string[1024];
    memcpy( input_string, str, str_len );
    input_string[str_len]='\0';
    switch ( type )
    {
    case US_GETOPT_NONE:
        break;
    case US_GETOPT_FLAG:
        *(bool *)value = true;
        break;
    case US_GETOPT_CHAR:
        *(char *)value = input_string[0];
        break;
    case US_GETOPT_INT16:
        *(int16_t *)value = (int16_t)strtol( input_string, 0, 10 );
        break;
    case US_GETOPT_UINT16:
        *(uint16_t *)value = (uint16_t)strtoul( input_string, 0, 10 );
        break;
    case US_GETOPT_INT32:
        *(int32_t *)value = (int32_t)strtol( input_string, 0, 10 );
        break;
    case US_GETOPT_UINT32:
        *(uint32_t *)value = (uint32_t)strtoul( input_string, 0, 10 );
        break;
    case US_GETOPT_HEX16:
        *(uint16_t *)value = (uint16_t)strtoul( input_string, 0, 16 );
        break;
    case US_GETOPT_HEX32:
        *(uint32_t *)value = (uint32_t)strtoul( input_string, 0, 16 );
        break;
    case US_GETOPT_HEX64:
#ifdef _MSC_VER
    {
        uint32_t *low_part = (uint32_t *)value;
        uint32_t *hi_part = low_part+1;
        int cnt = sscanf( input_string, "%8x%8x", hi_part, low_part );
        if ( cnt!=2 )
        {
            r=false;
        }
    }
#else
    *(uint64_t *)value = (uint64_t)strtoull( input_string, 0, 16 );
#endif
    break;
    case US_GETOPT_MACADDR:
    {
        int32_t values[6];
        int cnt;
        cnt = sscanf( input_string, "%2x:%2x:%2x:%2x:%2x:%2x",
                      &values[0],
                      &values[1],
                      &values[2],
                      &values[3],
                      &values[4],
                      &values[5]
                    );
        if ( cnt==6 )
        {
            uint8_t *macaddr = (uint8_t *)value;
            macaddr[0] = (uint8_t)values[0];
            macaddr[1] = (uint8_t)values[1];
            macaddr[2] = (uint8_t)values[2];
            macaddr[3] = (uint8_t)values[3];
            macaddr[4] = (uint8_t)values[4];
            macaddr[5] = (uint8_t)values[5];
        }
        else
        {
            r=false;
        }
    }
    break;
    case US_GETOPT_STRING:
        us_delete( allocator, *(char **)value );
        *(char **)value = us_strdup( allocator, str );
        break;
#ifdef US_ENABLE_FLOAT
    case US_GETOPT_FLOAT:
        *(float *)value = (float)atof( str );
        break;
#endif
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

static void us_getopt_option_list_destroy( us_getopt_t *self, us_getopt_option_list_t *cur, us_allocator_t *allocator )
{
    us_getopt_option_t *option;
    option=(us_getopt_option_t *)cur->m_options;
    while ( option && option->m_name )
    {
        if ( option->m_value_type && option->m_value_type==US_GETOPT_STRING && option->m_current_value )
        {
            char **pp = (char **)option->m_current_value;
            us_delete( allocator, *pp );
            *pp=0;
        }
        option++;
    }
    if ( cur->m_next )
    {
        us_getopt_option_list_destroy( self, cur->m_next, allocator );
        cur->m_next = 0;
    }
    us_delete( self->m_allocator, cur );
}

void us_getopt_destroy( us_getopt_t *self )
{
    us_getopt_option_list_t *cur = self->m_option_lists;
    if ( cur )
    {
        us_getopt_option_list_destroy( self, cur, self->m_allocator );
    }
    self->m_option_lists = 0;
}

bool us_getopt_add_list( us_getopt_t *self, const us_getopt_option_t *options, const char *prefix, const char *description )
{
    bool r=false;
    us_getopt_option_list_t *p = 0;
    p = us_new( self->m_allocator, us_getopt_option_list_t );
    if ( p )
    {
        p->m_options = options;
        p->m_next = 0;
        p->m_prefix = prefix;
        p->m_description = description;
        r=true;
        if ( self->m_last_option_list )
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
    while ( list && r)
    {
        const us_getopt_option_t *opt = list->m_options;
        while ( opt && r && opt->m_name!=0 )
        {
            us_getopt_copy_value( self->m_allocator, opt->m_current_value, opt->m_value_type, opt->m_default_value );
            opt++;
        }
        list=list->m_next;
    }
    return r;
}

bool us_getopt_print( us_getopt_t *self, us_print_t *printer )
{
    return us_getopt_dump(self,printer,0);
}

bool us_getopt_dump( us_getopt_t *self, us_print_t *printer, const char *ignore_key )
{
    bool r=true;
    us_getopt_option_list_t *list = self->m_option_lists;
    while ( list && r)
    {
        const us_getopt_option_t *opt = list->m_options;
        if ( list->m_prefix )
        {
            r&=printer->printf( printer,"#\n# Option group '%s': %s\n#\n\n", list->m_prefix, list->m_description );
        }
        while ( opt && r && opt->m_name!=0 )
        {
            char key_name[1024] = "";
            char value_string[1024] = "";
            us_getopt_string_for_value( value_string, sizeof(value_string)-1, opt->m_value_type, opt->m_current_value );
            if ( list->m_prefix )
            {
                sprintf(key_name,"%s.%s", list->m_prefix, opt->m_name );
            }
            else
            {
                strcpy( key_name, opt->m_name );
            }
            if (! ( ignore_key && strcmp(key_name,ignore_key)==0) )
            {
                if ( opt->m_value_type == US_GETOPT_FLAG )
                {
                    if ( *(bool *)opt->m_current_value == true )
                    {
                        r&=printer->printf(
                               printer,
                               "# %s (%s) : %s\n",
                               key_name,
                               us_getopt_value_types[opt->m_value_type],
                               opt->m_description
                           );
                        r&=printer->printf(
                               printer,
                               "%s = \n\n",
                               key_name
                           );
                    }
                    else
                    {
                        r&=printer->printf(
                               printer,
                               "\n\n"
                           );
                    }
                }
                else
                {
                    r&=printer->printf(
                           printer,
                           "# %s (%s) : %s\n",
                           key_name,
                           us_getopt_value_types[opt->m_value_type],
                           opt->m_description
                       );
                    r&=printer->printf(
                           printer,
                           "%s = \"%s\"\n\n",
                           key_name,
                           value_string
                       );
                }
            }
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
    while ( list && !r)
    {
        int prefix_len = 0;
        const char *subname = name;
        int subname_len=name_len;
        if ( list->m_prefix!=0 )
        {
            prefix_len = (int32_t)strlen( list->m_prefix );
            subname+=(prefix_len+1);
            subname_len-=(prefix_len+1);
        }
        if ( subname_len>0 && (prefix_len==0 || strncmp( name, list->m_prefix, prefix_len )==0 ) )
        {
            const us_getopt_option_t *opt = list->m_options;
            while ( opt && opt->m_name!=0 )
            {
                if ( strncmp( subname, opt->m_name, subname_len ) == 0 )
                {
                    r=us_getopt_value_for_string(
                          self->m_allocator,
                          opt->m_value_type,
                          opt->m_current_value,
                          value,
                          value_len
                      );
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
    while ( *argv )
    {
        if ( (*argv)[0]=='-' && (*argv)[1]=='-' )
        {
            const char *pos_equals = strchr( *argv, '=' );
            const char *pos_name = (*argv)+2;
            int name_len = (pos_equals!=0 ? (int32_t)(pos_equals-pos_name) : (int32_t)strlen(pos_name) );
            const char *pos_value = (pos_equals ? pos_equals+1 : 0 );
            int value_len = (pos_value ? (int32_t)strlen(pos_value) : 0 );
            r&=us_getopt_parse_one( self, pos_name, name_len, pos_value, value_len );
        }
        else
        {
            if ( !us_getopt_parse_file( self, *argv ) )
            {
                us_log_error( "Unable to parse options file: %s", *argv );
            }
        }
        ++argv;
    }
    return r;
}

bool us_getopt_parse_file( us_getopt_t *self, const char *fname )
{
    bool r=false;
    FILE *f = fopen( fname, "rt" );
    if ( f )
    {
        char line[2048];
        while ( fgets( line, sizeof(line), f ) )
        {
            size_t line_len = strlen(line);
            r&=us_getopt_parse_line( self, line, line_len );
        }
        fclose(f);
        r=true;
    }
    return r;
}

bool us_getopt_parse_line( us_getopt_t *self, const char *line, size_t line_len )
{
    bool r=true;
    size_t i;
    char escaped_key_name[128]="";
    size_t escaped_key_name_len=0;
    char *escaped_key_ptr = &escaped_key_name[0];
    char escaped_value[2048]="";
    size_t escaped_value_len=0;
    char *escaped_value_ptr = &escaped_value[0];
    char unescaped_key_name[128]="";
    char unescaped_value[2048]="";
    enum
    {
        IN_KEY_PREFIX=0,
        IN_KEY_NAME,
        IN_KEY_SUFFIX,
        IN_VALUE_PREFIX,
        IN_VALUE,
        IN_VALUE_SUFFIX
    } state;
    state = IN_KEY_PREFIX;
    for ( i=0; i<line_len; ++i )
    {
        char c = line[i];
        switch ( state )
        {
        case IN_KEY_PREFIX:
            if ( !isspace(c) && isprint(c))
            {
                state = IN_KEY_NAME;
                escaped_key_name[escaped_key_name_len++] = c;
            }
            break;
        case IN_KEY_NAME:
            if ( c=='=' )
            {
                state = IN_VALUE_PREFIX;
            }
            else if ( !isspace(c) && isprint(c) )
            {
                if ( escaped_key_name_len< sizeof(escaped_key_name)-1 )
                {
                    escaped_key_name[ escaped_key_name_len++ ] = c;
                    escaped_key_name[ escaped_key_name_len ] = '\0';
                }
            }
            else
            {
                state = IN_KEY_SUFFIX;
            }
            break;
        case IN_KEY_SUFFIX:
            if ( c=='=' )
            {
                state = IN_VALUE_PREFIX;
            }
            break;
        case IN_VALUE_PREFIX:
            if ( !isspace(c) && isprint(c) )
            {
                state = IN_VALUE;
                escaped_value[escaped_value_len++] = c;
            }
            break;
        case IN_VALUE:
            if ( isprint(c) )
            {
                if ( escaped_value_len<sizeof(escaped_value)-1 )
                {
                    escaped_value[ escaped_value_len++ ] = c;
                    escaped_value[ escaped_value_len ] = '\0';
                }
            }
            else
            {
                state = IN_VALUE_SUFFIX;
            }
            break;
        case IN_VALUE_SUFFIX:
            break;
        }
    }
    if( state!=IN_VALUE_SUFFIX)
    {
        return false;
    }
    /* remove trailing whitespace */
    for (i=escaped_value_len-1; i>0; --i)
    {
        if ( isspace(escaped_value[i]) )
        {
            escaped_value[i]='\0';
            escaped_value_len=i;
        }
        else
        {
            break;
        }
    }
    /* remove optional surrounding " from escaped key and value */
    if ( escaped_key_name_len>0 &&
            escaped_key_name[0] == '"' &&
            escaped_key_name[ escaped_key_name_len-1 ] == '"' )
    {
        escaped_key_name[escaped_key_name_len-1] = '\0';
        escaped_key_ptr++;
        escaped_key_name_len-=2;
    }
    if ( escaped_value_len>0 &&
            escaped_value[0] == '"' &&
            escaped_value[ escaped_value_len-1 ] == '"' )
    {
        escaped_value[escaped_value_len-1] = '\0';
        escaped_value_ptr++;
        escaped_value_len-=2;
    }
    /* if key starts with '#' or ';' then the line is actually a comment */
    if ( *escaped_key_ptr!='#' && *escaped_key_ptr!=';')
    {
        /* pass unescaped key/value to us_get_opt_parse_one */
        us_getopt_unescape( unescaped_key_name, sizeof(unescaped_key_name)-1, escaped_key_ptr, escaped_key_name_len);
        us_getopt_unescape( unescaped_value, sizeof(unescaped_value)-1, escaped_value_ptr, escaped_value_len );
        r=us_getopt_parse_one(
              self,
              unescaped_key_name, (int32_t)strlen(unescaped_key_name),
              unescaped_value, (int32_t)strlen( unescaped_value )
          );
    }
    return r;
}

