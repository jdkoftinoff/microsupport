#include "us_world.h"
#include "us_net.h"
#include "us_parse.h"

/**
 \addtogroup us_example_hex_udp_tx
 @{
 */


int main ( int argc, char **argv )
{
    int r = 0;
    const char * dest_host = "::1";
    const char * dest_port = "10000";
    struct addrinfo *dest_ai = 0;
    const char * src_host = "::1";
    const char * src_port = "10001";
    struct addrinfo *src_ai = 0;
    const char *ascii_hex_data = "414243";
    uint8_t data[1500];
    size_t data_len = 0;
    size_t pos;
    size_t len;
    if ( argc == 2 && strcmp ( "--help", argv[1] ) == 0 )
    {
        fprintf ( stdout, "usage:\n\t%s {hexbytes} {dest ip} {dest port} {src ip} {src port}\n"
                  "dest defaults to: %s %s\n"
                  "src defaults to %s %s\n",
                  argv[0],
                  dest_host,
                  dest_port,
                  src_host,
                  src_port
                );
        exit ( 1 );
    }
    if ( argc > 1 )
    {
        ascii_hex_data = argv[1];
    }
    if ( argc > 2 )
    {
        dest_host = argv[2];
    }
    if ( argc > 3 )
    {
        dest_port = argv[3];
    }
    if ( argc > 4 )
    {
        src_host = argv[4];
    }
    if ( argc > 5 )
    {
        src_port = argv[5];
    }
    len = strlen ( ascii_hex_data );
    pos = 0;
    while ( us_parse_hexoctet ( &data[ data_len ], ascii_hex_data, len, &pos ) )
    {
        data_len++;
    }
    dest_ai = us_net_get_addrinfo ( dest_host, dest_port, SOCK_DGRAM, false );
    src_ai = us_net_get_addrinfo ( src_host, src_port, SOCK_DGRAM, false );
    if ( dest_ai && src_ai )
    {
        int e;
        int outgoing_sock_fd;
        outgoing_sock_fd = us_net_create_udp_socket ( src_ai, true );
        if ( outgoing_sock_fd == -1 )
        {
            perror ( "opening outgoing socket:" );
            return 1;
        }
        do
        {
            e = sendto( outgoing_sock_fd, data, data_len, 0, dest_ai->ai_addr, dest_ai->ai_addrlen );
        }
        while ( e==-1 && errno == EINTR );
        if ( e==-1 )
        {
            perror ( "error sending udp packet" );
            r=1;
        }
        closesocket ( outgoing_sock_fd );
    }
    else
    {
        perror ( "getting addresses:" );
    }
    return r;
}

/*@}*/
