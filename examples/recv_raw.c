#include "us_world.h"
#include "us_bits.h"
#include "us_rawnet.h"



int main(int argc, const char **argv )
{
    int interface_id=0;
    uint8_t multicast_mac[6] = { 0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00 };
    uint16_t ethertype=0x22f0;
    const char *ifname = "eth1";
    if( argc>1 )
        ifname=argv[1];
    int fd=us_rawnet_socket( ethertype, 0, &interface_id, ifname );
    if( fd>=0 )
    {
        uint8_t pkt_src_mac[6];
        uint8_t pkt_dest_mac[6];
        uint16_t pkt_ethertype;
        int pkt_if;
        uint8_t buf[2048];
        if( !us_rawnet_join_multicast(
                    fd,
                    interface_id,
                    ethertype,
                    multicast_mac
                ) )
        {
            exit(1);
        }
        ssize_t buf_len=0;
        do
        {
            buf_len=us_rawnet_recv(
                        fd,
                        &pkt_if,
                        pkt_src_mac,
                        pkt_dest_mac,
                        &pkt_ethertype,
                        buf, sizeof(buf)
                    );
            if( buf_len>=0 )
            {
                int i;
                printf( "Received interface %d EtherType 0x%04x packet multicast addr %02x:%02x:%02x:%02x:%02x:%02x payload len=%d from: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        pkt_if, pkt_ethertype,
                        pkt_dest_mac[0], pkt_dest_mac[1], pkt_dest_mac[2], pkt_dest_mac[3], pkt_dest_mac[4], pkt_dest_mac[5],
                        (int)buf_len,
                        pkt_src_mac[0], pkt_src_mac[1], pkt_src_mac[2], pkt_src_mac[3], pkt_src_mac[4], pkt_src_mac[5]
                      );
                for( i=0; i<buf_len; ++i )
                {
                    printf( "%02x ", buf[i] );
                }
                printf( "\n\n" );
            }
        }
        while( buf_len>=0 );
    }
}

