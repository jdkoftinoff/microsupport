#include "us_world.h"
#include "us_allocator.h"
#include "us_buffer.h"
#include "us_net.h"
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


/** \addtogroup us_test_net */
/*@{*/

bool us_example_mudp_rx (
    const char *multicast_address,
    const char *multicast_service,
    const char *interface
);

bool us_example_mudp_rx (
    const char *multicast_address,
    const char *multicast_service,
    const char *interface
)
{
    int fd;
    bool r = false;
    struct addrinfo *multicastgroup;
    multicastgroup = us_net_get_addrinfo ( multicast_address, multicast_service, SOCK_DGRAM );
    {
        char multicastgroup_name[1024];
        char multicastgroup_serv[256];
        
        if ( us_net_get_nameinfo ( multicastgroup, multicastgroup_name, sizeof ( multicastgroup_name ) - 1, multicastgroup_serv, sizeof ( multicastgroup_serv ) - 1 ) )
        {
            us_log_info ( "multicastgroup: %s port %s", multicastgroup_name, multicastgroup_serv );
        }
        
        else
        {
            perror ( "getnameinfo:" );
        }
    }
    fd = us_net_create_multicast_rx_udp_socket ( 0, multicastgroup, interface );
    
    if ( fd > 0 )
    {
        while ( 1 )
        {
            struct sockaddr_storage remote_addr;
            socklen_t remote_addr_len = sizeof ( remote_addr );
            char buf[2048];
            int len;
            len = recvfrom ( fd, buf, sizeof ( buf ), 0, ( struct sockaddr * ) &remote_addr, &remote_addr_len );
            
            if ( len > 0 )
            {
                us_log_info ( "got packet %d bytes", len );
            }
            
            else
            {
                perror ( "recvfrom:" );
                break;
            }
        }
    }
    
    return r;
}


int main ( int argc, char **argv )
{
    int r = 1;
    
    if ( argc < 3 )
    {
        fprintf ( stderr, "usage:\n\t%s multicast_address multicast_service (interface)\n", argv[0] );
        fprintf ( stderr, "example:\n\t%s ff31::8000:1234 30001\n", argv[0] );
        exit ( 1 );
    }
    
    if ( us_testutil_start ( 4096, 4096, argc, argv ) )
    {
        const char *multicast_address = argv[1];
        const char *multicast_service = argv[2];
        const char *interface = "";
        
        if ( argc > 3 )
            interface = argv[3];
            
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        us_log_info ( "Hello world from %s compiled on %s", __FILE__, __DATE__ );
        
        if ( us_example_mudp_rx ( multicast_address, multicast_service, interface ) )
            r = 0;
            
        us_log_info ( "Finishing %s", argv[0] );
        us_logger_finish();
        us_testutil_finish();
    }
    
    return r;
}

/*@}*/
