#include "us_world.h"

#include "us_rawnet.h"


int main( int argc, char **argv )
{
    const char *if_name="eth0";
    uint8_t dest_mac[6] = { 0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00 };
	int fd;
	us_rawnet_context_t sock;
    if( argc>1 )
    {
        if_name=argv[1];
    }
    fd=us_rawnet_socket( &sock, 0x22f0, if_name );
    if( fd>=0 )
    {
        int i=0;
        while(true)
        {
            uint8_t buf[1500];
            size_t buflen=sizeof(buf);
            for( int i=0; i<buflen; ++i )
            {
                buf[i] = (uint8_t)i&0xff;
            }
            if( us_rawnet_send( &sock, dest_mac, buf, buflen )>=0 )
            {
                printf( "sent packet %d\n", i++ );
            }
            sleep(1);
        }
    }
}

