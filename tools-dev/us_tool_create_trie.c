#include "us_world.h"
#include "us_testutil.h"
#include "us_logger.h"
#include "us_logger_printer.h"
#include "us_trie_dump.h"

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
  * Neither the name of Meyer Sound Laboratories, Inc. nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC.  BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if US_ENABLE_STDIO

typedef struct func_name_list_s
{
    struct func_name_list_s *m_next;
    const char func_name[256];
} func_name_list_t;


void usage ( void );
us_trie_dyn_t *us_tool_read_trie (
    FILE *input_file,
    func_name_list_t **func_list,
    int max_nodes
);

bool us_tool_create_trie_f (
    const char *prefix,
    const char *input_filename,
    const char *output_c_filename,
    const char *output_h_filename,
    FILE *input_file,
    FILE *output_c_file,
    FILE *output_h_file
);
bool us_tool_create_trie (
    const char *prefix,
    const char *input_filename,
    const char *output_c_filename,
    const char *output_h_filename
);

void usage ( void )
{
    us_testutil_printer_stderr->printf (
        us_testutil_printer_stderr,
        "us_tool_create_trie usage:\n"
        "\tus_tool_create_trie prefix input_schema.txt output.c output.h\n\n"
    );
}

us_trie_dyn_t *us_tool_read_trie (
    FILE *input_file,
    func_name_list_t **func_list,
    int max_nodes
)
{
    us_trie_node_flags_t item=0;
    us_trie_dyn_t *trie;
    func_name_list_t **cur_func = func_list;
    trie = us_trie_dyn_create (
               us_testutil_sys_allocator,
               max_nodes,
               us_trie_basic_ignorer,
               us_trie_basic_ignorer,
               us_trie_basic_db_skip,
               us_trie_basic_comparator
           );
    if ( trie )
    {
        while ( !feof ( input_file ) )
        {
            char line[1024];
            char *l = fgets ( line, sizeof ( line ), input_file );
            if ( l )
            {
                char *first_space = strchr ( l, ' ' );
                char *func_name = first_space + 1;
                if ( first_space )
                    *first_space = '\0';
                *cur_func = (func_name_list_t *)calloc ( sizeof ( func_name_list_t ), 1 );
                strcpy ( ( char * ) ( *cur_func )->func_name, func_name );
                us_trie_add (
                    &trie->m_base,
                    ( us_trie_node_value_t* ) l,
                    strlen ( l ),
                    item
                );
                cur_func = & ( *cur_func )->m_next;
                item++;
            }
        }
    }
    return trie;
}

bool us_tool_create_trie_f (
    const char *prefix,
    const char *input_filename,
    const char *output_c_filename,
    const char *output_h_filename,
    FILE *input_file,
    FILE *output_c_file,
    FILE *output_h_file
)
{
    bool r = false;
    func_name_list_t *func_names = 0;
    us_trie_dyn_t *trie =  us_tool_read_trie ( input_file, &func_names, 2048 );
    if ( trie && func_names )
    {
        int item;
        fprintf ( output_c_file, "#include \"%s\"\n\n",  output_h_filename );
        fprintf ( output_c_file, "us_trie_node_t %s_nodes[] = {\n", prefix );
        for ( item = 0; item < trie->m_base.m_num_nodes; ++item )
        {
            us_trie_node_t *n = &trie->m_base.m_nodes[item];
            fprintf (
                output_c_file,
                "  { %d, %d, %d, %d, %d },\n",
                n->m_parent, n->m_sibling, n->m_child, n->m_value, n->m_flags
            );
        }
        fprintf ( output_c_file, "};\n\n" );
        {
            func_name_list_t *cur = func_names;
            fprintf ( output_c_file, "us_trie_func_t %s_funcs[] = {\n", prefix );
            while ( cur )
            {
                fprintf ( output_c_file, "  %s,\n", cur->func_name );
                cur = cur->m_next;
            }
            fprintf ( output_c_file, "};\n\n" );
        }
        r = true;
    }
    if ( trie )
    {
        trie->destroy ( trie );
    }
    return r;
}


bool us_tool_create_trie (
    const char *prefix,
    const char *input_filename,
    const char *output_c_filename,
    const char *output_h_filename
)
{
    bool r = true;
    FILE *input_file = 0;
    FILE *output_c_file = 0;
    FILE *output_h_file = 0;
    input_file = fopen ( input_filename, "rt" );
    output_c_file = fopen ( output_c_filename, "wt" );
    output_h_file = fopen ( output_h_filename, "wt" );
    if ( !input_file )
    {
        r = false;
        us_testutil_printer_stderr->printf (
            us_testutil_printer_stderr,
            "Error Opening input file: '%s'\n",
            input_filename
        );
    }
    if ( !output_c_file )
    {
        r = false;
        us_testutil_printer_stderr->printf (
            us_testutil_printer_stderr,
            "Error Opening output c file: '%s'\n",
            output_c_filename
        );
    }
    if ( !output_h_file )
    {
        r = false;
        us_testutil_printer_stderr->printf (
            us_testutil_printer_stderr,
            "Error Opening output h file: '%s'\n",
            output_h_filename
        );
    }
    if ( r )
        r = us_tool_create_trie_f (
                prefix,
                input_filename,
                output_c_filename,
                output_h_filename,
                input_file,
                output_c_file,
                output_h_file
            );
    if ( input_file )
        fclose ( input_file );
    if ( output_c_file )
        fclose ( output_c_file );
    if ( output_h_file )
        fclose ( output_h_file );
    return r;
}


int main ( int argc, const char **argv )
{
    int r = 1;
    const char *prefix = "schema";
    const char *input_filename = "input_schema.txt";
    const char *output_c_filename = "output.c";
    const char *output_h_filename = "output.h";
    if ( argc > 1 )
        prefix = argv[1];
    if ( argc > 2 )
        input_filename = argv[2];
    if ( argc > 3 )
        output_c_filename = argv[3];
    if ( argc > 4 )
        output_h_filename = argv[4];
    if ( us_testutil_start ( 8192, 8192, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
#if 0
        if ( argc != 5 )
        {
            usage();
        }
        else
#endif
        {
            if ( us_tool_create_trie ( prefix, input_filename, output_c_filename, output_h_filename ) )
                r = 0;
        }
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}

#else

int main ( int argc, char **argv )
{
    return 0;
}

#endif
