#include "us_world.h"

#include "us_rawnet.h"


int main( int argc, char **argv )
{
#if defined(US_ENABLE_RAW)
#if defined (WIN32)
#define sleep(x) Sleep(x*1000)
    const char *if_name="\\Device\\NPF_{BD3BC03E-E00B-490E-97C4-7A01467918D4}";
#else
    const char *if_name="eth0";
#endif
    uint8_t dest_mac[6] = { 0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00 };
    int fd;
    us_rawnet_context_t sock;
    if( argc>1 )
    {
        if_name=argv[1];
    }
    fd=us_rawnet_socket( &sock, 0x22f0, if_name, dest_mac );
    if( fd>=0 )
    {
        int i=0;
        int rep=0;
        for( rep=0; rep<10; ++rep )
        {
            uint8_t buf[1500];
            size_t buflen=sizeof(buf);
            for( i=0; i<buflen; ++i )
            {
                buf[i] = (uint8_t)i&0xff;
            }
            if( us_rawnet_send( &sock, dest_mac, buf, buflen )>=0 )
            {
                printf( "sent packet %d\n", rep );
            }
            sleep(1);
        }
    }
#endif
}

