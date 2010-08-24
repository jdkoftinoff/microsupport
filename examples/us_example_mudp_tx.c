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

bool us_example_mudp_tx( int argc, char **argv )
{
    int fd;
    bool r=false;
    struct addrinfo *multicastgroup;
    const char *multicastgroup_host = argv[1];
    const char *multicastgroup_serv = argv[2];
    struct addrinfo *listenaddr;
    const char *listenaddr_host = argv[3];
    const char *listenaddr_serv = argv[4];
    const char *interface_name = argv[5];

    multicastgroup = us_net_get_addrinfo( multicastgroup_host, multicastgroup_serv, SOCK_DGRAM );
    listenaddr = us_net_get_addrinfo( listenaddr_host, listenaddr_serv, SOCK_DGRAM );

    {
        char multicastgroup_name[1024];
        char multicastgroup_serv[256];
        char listenaddr_name[1024];
        char listenaddr_serv[1024];

        if( getnameinfo(
                    listenaddr->ai_addr, listenaddr->ai_addrlen, listenaddr_name,
                    sizeof(listenaddr_name)-1, listenaddr_serv,
                    sizeof(listenaddr_serv)-1, NI_NUMERICHOST | NI_NUMERICSERV
                        )==0 )
        {
            us_log_info( "listenaddr: %s port %s", listenaddr_name, listenaddr_serv );
        }
        else
        {
            perror( "getnameinfo:" );
        }

        if( getnameinfo(
                    multicastgroup->ai_addr, multicastgroup->ai_addrlen, multicastgroup_name,
                    sizeof(multicastgroup_name)-1, multicastgroup_serv,
                    sizeof(multicastgroup_serv)-1, NI_NUMERICHOST | NI_NUMERICSERV )==0 )
        {
            us_log_info( "listenaddr: %s port %s", multicastgroup_name, multicastgroup_serv );
        }
        else
        {
            perror( "getnameinfo:" );
        }

    }


    fd = us_net_create_multicast_udp_socket( listenaddr, multicastgroup, interface_name );
    if( fd>0 )
    {
        while(1)
        {
            struct sockaddr_storage remote_addr;
            socklen_t remote_addr_len = sizeof(remote_addr);
            char buf[2048];
            int len;
            len = recvfrom( fd, buf, sizeof(buf), 0, (struct sockaddr *)&remote_addr, &remote_addr_len );
            if( len>0 )
            {
                us_log_info( "got packet %d bytes", len );
            }
            else
            {
                perror("recvfrom:" );
                break;
            }
        }
    }

    return r;
}


int main( int argc, char **argv )
{
  int r=1;
  if( us_testutil_start(4096,4096,argc,argv) )
  {
#if US_ENABLE_LOGGING
    us_logger_printer_start( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif

    us_log_set_level( US_LOG_LEVEL_DEBUG );
    us_log_info( "Hello world from %s compiled on %s", __FILE__, __DATE__ );

    if( us_example_mudp_tx(argc,argv) )
      r=0;

    us_log_info("Finishing %s", argv[0] );
    us_logger_finish();
    us_testutil_finish();
  }
  return r;
}

/*@}*/
