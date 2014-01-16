#include "us_world.h"
#include "us_bits.h"
#include "us_rawnet.h"



int main(int argc, const char **argv )
{
    int result = 255;
#if US_ENABLE_RAW_ETHERNET==1
    uint8_t multicast_mac[6] = { 0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00 };
    uint16_t ethertype=0x22f0;
    us_rawnet_context_t sock;
#if defined (WIN32)
#define sleep(x) Sleep(x*1000)
    const char *ifname="\\Device\\NPF_{BD3BC03E-E00B-490E-97C4-7A01467918D4}";
#else
    const char *ifname="eth1";
#endif

    int fd;
    if( argc>1 )
        ifname=argv[1];
    fd=us_rawnet_socket( &sock, ethertype, ifname, multicast_mac );
    if( fd>=0 )
    {
        uint8_t pkt_src_mac[6];
        uint8_t pkt_dest_mac[6];
        uint8_t buf[2048];
        ssize_t buf_len=0;
        result=0;
        do
        {
            buf_len=us_rawnet_recv(
                        &sock,
                        pkt_src_mac,
                        pkt_dest_mac,
                        buf, sizeof(buf)
                    );
            if( buf_len>=0 )
            {
                int i;
                printf( "Received packet payload len=%d from: %02x:%02x:%02x:%02x:%02x:%02x to: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        (int)buf_len,
                        pkt_src_mac[0], pkt_src_mac[1], pkt_src_mac[2], pkt_src_mac[3], pkt_src_mac[4], pkt_src_mac[5],
                        pkt_dest_mac[0], pkt_dest_mac[1], pkt_dest_mac[2], pkt_dest_mac[3], pkt_dest_mac[4], pkt_dest_mac[5]
                      );
                for( i=0; i<buf_len; ++i )
                {
                    printf( "%02x ", buf[i] );
                }
                printf( "\n\n" );
            }
            else
            {
                sleep(1);
            }
        }
        while( true );
    }
#endif
    return result;
}

