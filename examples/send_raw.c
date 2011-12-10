#include "us_world.h"

#include "us_rawnet.h"


int main( int argc, char **argv )
{
#ifdef __linux__
    const char *if_name="eth0";
    int if_id=0;
    uint8_t my_mac[6];
    uint8_t dest_mac[6] = { 0x91, 0xe0, 0xf0, 0x00, 0x00, 0x1 };

	if( argc>1 )
	{
		if_name=argv[1];
	}

    int fd=us_rawnet_socket( 0x22f0, my_mac, &if_id, if_name );
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
            if( us_rawnet_send( fd, if_id, my_mac, dest_mac, 0x22f0, buf, buflen )>=0 )
            {
                printf( "sent packet %d\n", i++ );
            }

            sleep(1);
        }
    }
    perror("Done:");
#endif
}

