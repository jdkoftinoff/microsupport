#include "us_world.h"
#include <linux/if_packet.h>

#include "us_rawnet.h"


int main()
{
#ifdef __linux__
    int fd=socket(AF_PACKET,SOCK_RAW,htons(0x22f0) );
    if( fd>=0 )
    {
        uint8_t buf[2048];
        struct sockaddr_ll src_addr;
        socklen_t src_addr_len = sizeof(src_addr);
        int buf_len=0;
        do 
        {
            buf_len=recvfrom( fd, buf, sizeof(buf), 0, (struct sockaddr *)&src_addr, &src_addr_len );
            if( buf_len>=0 )
            {
                int i;
                printf( "Received EtherType 22f0 packet len=%d from: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        buf_len,
                        src_addr.sll_addr[0],
                        src_addr.sll_addr[1],
                        src_addr.sll_addr[2],
                        src_addr.sll_addr[3],
                        src_addr.sll_addr[4],
                        src_addr.sll_addr[5]
                      );
                for( i=0; i<buf_len; ++i )
                {
                    printf( "%02x ", buf[i] );
                }
                printf( "\n\n" );
            }
        } while( buf_len>0 );
    }
    perror("done");
#endif
}

