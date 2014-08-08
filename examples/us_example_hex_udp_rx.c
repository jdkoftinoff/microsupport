#include "us_world.h"
#include "us_net.h"

/**
 \addtogroup us_example_hex_udp_rx
 @{
 */


int main ( int argc, char **argv )
{
    int r = 1;
    const char * listen_host = "::1";
    const char * listen_port = "10000";
    struct addrinfo *listen_ai = 0;
    if ( argc == 2 && strcmp ( "--help", argv[1] ) == 0 )
    {
        fprintf ( stdout, "usage:\n\t%s {local ip} {local port}\ndefaults to: %s %s\n",
                  argv[0],
                  listen_host,
                  listen_port
                );
        exit ( 1 );
    }
    if ( argc > 1 )
    {
        listen_host = argv[1];
    }
    if ( argc > 2 )
    {
        listen_port = argv[2];
    }
    listen_ai = us_net_get_addrinfo ( listen_host, listen_port, SOCK_DGRAM, true );
    if ( listen_ai )
    {
        int incoming_sock_fd;
        incoming_sock_fd = us_net_create_udp_socket ( listen_ai, true );
        if ( incoming_sock_fd == -1 )
        {
            perror ( "opening incoming socket:" );
            return 1;
        }
        while ( true )
        {
            struct sockaddr_storage remote_addr;
            socklen_t remote_addr_len;
            char hostname_buf[NI_MAXHOST];
            char serv_buf[NI_MAXSERV];
            uint8_t pkt_buf[1536];
            ssize_t pkt_len;
            int i;
            int e;
            do
            {
                remote_addr_len = sizeof( remote_addr );
                pkt_len = recvfrom (
                              incoming_sock_fd,
                              (char *)&pkt_buf, sizeof ( pkt_buf ),
                              0,
                              ( struct sockaddr * ) & remote_addr, &remote_addr_len
                          );
            }
            while ( pkt_len < 0 && errno == EINTR );
            if ( pkt_len < 0 )
            {
                perror ( "recvfrom:" );
                break;
            }
            e = getnameinfo(
                    (struct sockaddr *) &remote_addr, remote_addr_len,
                    hostname_buf,
                    sizeof(hostname_buf)-1,
                    serv_buf,
                    sizeof(serv_buf)-1,
                    NI_NUMERICHOST | NI_NUMERICSERV | NI_DGRAM
                );
            if ( e==0 )
            {
                fprintf( stdout, "Packet From: %s port %s\n", hostname_buf, serv_buf );
            }
            else
            {
                fprintf( stderr, "getnameinfo failed: %s\n",gai_strerror( e ));
                break;
            }
            fprintf( stdout, "Packet Length: %d bytes\n", (int)pkt_len );
            fprintf( stdout, "Contents: " );
            for ( i = 0; i < pkt_len; ++i )
            {
                fprintf ( stdout, "%02x", pkt_buf[i] );
            }
            fprintf( stdout, "\n\n" );
        }
        closesocket ( incoming_sock_fd );
    }
    else
    {
        perror ( "getting addresses:" );
    }
    return r;
}

/*@}*/
