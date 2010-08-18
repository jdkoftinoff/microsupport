#include "us_world.h"
#include "us_allocator.h"
#include "us_trie.h"
#include "us_logger_printer.h"

#include "us_testutil.h"

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
 * Neither the name of the <organization> nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/** \addtogroup us_test_trie */
/*@{*/
bool us_test_trie_1 ( void );
bool us_test_trie_2 ( void );


bool us_test_trie_1 ( void )
{
    bool r = true;
    us_trie_dyn_t *trie;
    us_log_info ( "us_test_trie_1" );
    trie = us_trie_dyn_create (
               us_testutil_sys_allocator,
               128,
               us_trie_basic_ignorer,
               us_trie_basic_comparator
           );
           
    if ( trie )
    {
        const char *s;
        s = "/input/level/";
        us_trie_add (
            &trie->m_base,
            ( us_trie_node_value_t * ) s,
            strlen ( s ),
            ( us_trie_node_flags_t ) 1
        );
        s = "/output/level/";
        us_trie_add (
            &trie->m_base,
            ( us_trie_node_value_t * ) s,
            strlen ( s ),
            ( us_trie_node_flags_t ) 2
        );
        {
            us_trie_node_flags_t flags;
            int16_t match_len;
            us_trie_node_id_t match_item;
            s = "/output/level/";
            
            if ( us_trie_find (
                        &trie->m_base,
                        ( us_trie_node_value_t * ) s,
                        strlen ( s ),
                        &flags,
                        &match_len,
                        &match_item,
                        0,
                        0
                    ) )
            {
                us_log_info ( "found '%s' len %d flags %d", s, match_len, flags );
            }
            
            else
            {
                us_log_info ( "did not find '%s'", s );
                r = false;
            }
            
            s = "/input/level/";
            
            if ( us_trie_find (
                        &trie->m_base,
                        ( us_trie_node_value_t * ) s,
                        strlen ( s ),
                        &flags,
                        &match_len,
                        &match_item,
                        0,
                        0
                    ) )
            {
                us_log_info ( "found '%s' len %d flags %d", s, match_len, flags );
            }
            
            else
            {
                us_log_info ( "did not find '%s'", s );
                r = false;
            }
        }
        trie->destroy ( trie );
    }
    
    return r;
}


/* Experiments of ways of implementing schema tied to values and callbacks */

typedef enum
{
    US_RO = 0,
    US_RW = 1
} us_schema_rw_t;

typedef struct us_test_trie_schema_value_s
{
    char m_type;
    us_schema_rw_t m_rw;
    void *m_data;
    int16_t m_data_len;
} us_test_trie_schema_value_t;

typedef struct us_test_trie_schema_entry_s
{
    const char *m_address;
    us_test_trie_schema_value_t *m_params[4];
    int m_additional;
    bool ( *read_proc ) ( struct us_test_trie_schema_entry_s *self );
    bool ( *write_proc ) ( struct us_test_trie_schema_entry_s *self );
} us_test_trie_schema_entry_t;

char device_name[256] = "default device name";
char system_name[256] = "default system name";

us_test_trie_schema_value_t us_test_trie_value_device_name =
{
    's', US_RW, device_name, sizeof ( device_name )
};

us_test_trie_schema_value_t us_test_trie_value_system_name =
{
    's', US_RW, system_name, sizeof ( device_name )
};

bool device_name_read ( us_test_trie_schema_entry_t *self );
bool device_name_write ( us_test_trie_schema_entry_t *self );
bool system_name_read ( us_test_trie_schema_entry_t *self );
bool system_name_write ( us_test_trie_schema_entry_t *self );
bool us_test_trie_schema_dispatch ( us_trie_t *trie, const char *address, const char *types, void *value1, void *value2 );


us_test_trie_schema_entry_t us_test_trie_schema[] =
{
    { "/device/name", { &us_test_trie_value_device_name, 0 }, 0, device_name_read, device_name_write },
    { "/device/system", { &us_test_trie_value_system_name, 0 }, 0, system_name_read, system_name_write },
    { 0, {0}, 0, 0, 0 }
};


bool device_name_read ( us_test_trie_schema_entry_t *self )
{
    us_log_debug ( "%s: ", __FUNCTION__ );
    return true;
}


bool device_name_write ( us_test_trie_schema_entry_t *self )
{
    us_log_debug ( "%s: ", __FUNCTION__ );
    us_log_info ( "new system name: %s ", system_name );
    return true;
}


bool system_name_read ( us_test_trie_schema_entry_t *self )
{
    us_log_debug ( "%s: ", __FUNCTION__ );
    return true;
}


bool system_name_write ( us_test_trie_schema_entry_t *self )
{
    us_log_debug ( "%s: ", __FUNCTION__ );
    us_log_info ( "new system name: %s ", device_name );
    return true;
}

bool us_test_trie_schema_dispatch ( us_trie_t *trie, const char *address, const char *types, void *value1, void *value2 )
{
    bool r = false;
    us_trie_node_flags_t flags;
    int16_t match_len;
    us_trie_node_id_t match_item;
    
    if ( us_trie_find (
                trie,
                ( us_trie_node_value_t * ) address,
                strlen ( address ),
                &flags,
                &match_len,
                &match_item,
                0,
                0
            ) )
    {
        us_test_trie_schema_entry_t *entry = &us_test_trie_schema[ flags ];
        
        if ( types[0] == '\0' )
        {
            us_test_trie_schema_value_t *param = entry->m_params[0];
            
            if ( entry->read_proc )
            {
                r = entry->read_proc ( entry );
            }
            
            if ( param && param->m_type == 's' && param->m_data != 0 )
            {
                us_log_info ( "Read type s, value: %s", param->m_data );
            }
        }
        
        if ( types[0] == 's' )
        {
            us_test_trie_schema_value_t *param = entry->m_params[0];
            
            if ( param && param->m_rw == US_RW && param->m_type == 's' && param->m_data != 0 )
            {
                if ( ( int ) strlen ( ( const char * ) value1 ) < ( int ) ( param->m_data_len - 1 ) )
                {
                    strcpy ( param->m_data, ( const char * ) value1 );
                    
                    if ( entry->write_proc )
                    {
                        r = entry->write_proc ( entry );
                    }
                }
            }
        }
    }
    
    return r;
}

bool us_test_trie_2 ( void )
{
    bool r = false;
    us_trie_dyn_t *trie;
    us_log_info ( "us_test_trie_2" );
    trie = us_trie_dyn_create (
               us_testutil_sys_allocator,
               128,
               us_trie_basic_ignorer,
               us_trie_basic_comparator
           );
           
    if ( trie )
    {
        int item = 0;
        us_test_trie_schema_entry_t *cur = &us_test_trie_schema[0];
        
        while ( cur && cur->m_address != 0 )
        {
            us_trie_add (
                &trie->m_base,
                ( us_trie_node_value_t * ) cur->m_address,
                strlen ( cur->m_address ),
                ( us_trie_node_flags_t ) item
            );
            item++;
            cur++;
        }
        
        r = true;
        r &= us_test_trie_schema_dispatch ( &trie->m_base, "/device/name", "", 0, 0 );
        r &= us_test_trie_schema_dispatch ( &trie->m_base, "/device/system", "s", "new system name", 0 );
        r &= us_test_trie_schema_dispatch ( &trie->m_base, "/device/name", "s", "new device name", 0 );
        r &= us_test_trie_schema_dispatch ( &trie->m_base, "/device/system", "", 0, 0 );
        trie->destroy ( trie );
    }
    
    return r;
}


int main ( int argc, char **argv )
{
    int r = 1;
    
    if ( us_testutil_start ( 8192, 8192, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        
        if ( us_test_trie_1() && us_test_trie_2() )
            r = 0;
            
        if ( r != 0 )
            us_log_error ( "Failed" );
            
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    
    return r;
}


/*@}*/

