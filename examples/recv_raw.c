#include "us_world.h"
#include "us_bits.h"
#ifdef __linux__
#include <sys/ioctl.h>        // Provides function ioctl()
#include <linux/if_packet.h>
#endif
#include "us_rawnet.h"



int main(int argc, const char **argv )
{
#ifdef __linux__
    int interface_id=0;
    uint8_t multicast_mac[6] = { 0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00 };
    uint16_t ethertype=0x22f0;
    const char *ifname = "eth1";
    if( argc>1 )
        ifname=argv[1];
    int fd=us_rawnet_socket( ethertype, 0, &interface_id, ifname );
    if( fd>=0 )
    {
        uint8_t buf[2048];
        struct sockaddr_ll src_addr;
        if( !us_rawnet_join_multicast(
                    fd,
                    interface_id,
                    ethertype,
                    multicast_mac
                ) )
        {
            exit(1);
        }
        socklen_t src_addr_len = sizeof(src_addr);
        int buf_len=0;
        do
        {
            buf_len=recvfrom(
                        fd,
                        buf, sizeof(buf),
                        0,
                        (struct sockaddr *)&src_addr, &src_addr_len
                    );
            if( buf_len>=0 )
            {
                int i;
                printf( "Received EtherType 0x%04x packet multicast addr %02x:%02x:%02x:%02x:%02x:%02x payload len=%d from: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        US_BITS_MAKE_DOUBLET( buf[12], buf[13] ),
                        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
                        buf_len-14,
                        buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]
                      );
                for( i=14; i<buf_len; ++i )
                {
                    printf( "%02x ", buf[i] );
                }
                printf( "\n\n" );
            }
        }
        while( buf_len>0 );
    }
    perror("done");
#endif
}

