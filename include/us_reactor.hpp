#ifndef US_REACTOR_HPP
#define US_REACTOR_HPP

#include "us_world.hpp"
#include "us_reactor.h"
#include "us_reactor_handler_rawnet.h"
#include "us_reactor_handler_tcp.h"
#include "us_reactor_handler_udp.h"

namespace microsupport
{

class reactor_server_t
{
public:
    reactor_server_t ( us_allocator_t *allocator_, int max_handlers=16 )
        : allocator( allocator_ )
    {
        if( !us_reactor_init( &reactor, allocator, max_handlers ) )
        {
            throw std::bad_alloc();
        }
    }

    bool add_tcp_client(
        void *extra,
        size_t queue_buf_size,
        const char *server_host,
        const char *server_port,
        bool keep_open,
        us_reactor_handler_create_proc_t client_handler_create,
        us_reactor_handler_tcp_client_init_proc_t client_handler_init
        )
    {
        return us_reactor_create_tcp_client(
                    &reactor,
                    allocator,
                    extra,
                    queue_buf_size,
                    server_host,
                    server_port,
                    keep_open,
                    client_handler_create,
                    client_handler_init
                    );
    }

    bool add_tcp_server(
        const char *server_host,
        const char *server_port,
        void *client_extra,
        us_reactor_handler_create_proc_t server_handler_create,
        us_reactor_handler_init_proc_t server_handler_init
        )
    {
        return us_reactor_create_server(
                    &reactor,
                    allocator,
                    server_host,
                    server_port,
                    SOCK_STREAM,
                    client_extra,
                    server_handler_create,
                    server_handler_init
                    );
    }

    bool add_udp_server(
        const char *server_host,
        const char *server_port,
        void *client_extra,
        us_reactor_handler_create_proc_t server_handler_create,
        us_reactor_handler_init_proc_t server_handler_init
        )
    {
        return us_reactor_create_server(
                    &reactor,
                    allocator,
                    server_host,
                    server_port,
                    SOCK_DGRAM,
                    client_extra,
                    server_handler_create,
                    server_handler_init
                    );
    }

    bool add_rawnet_server(
        void *extra,
        const char *ethernet_port,
        uint16_t ethertype,
        const uint8_t multicast_mac[6],
        size_t input_packets,
        size_t output_packets
        )
    {
        bool r=false;
        us_reactor_handler_t *h = us_reactor_handler_rawnet_create(allocator);
        if( h )
        {
            if( us_reactor_handler_rawnet_init(
                        h,
                        allocator,
                        extra,
                        ethernet_port,
                        ethertype,
                        multicast_mac,
                        input_packets,
                        output_packets
                        ))
            {
                if( reactor.add_item( &reactor, h ) )
                {
                    r=true;
                }
                else
                {
                    h->destroy( h );
                }
            }
        }
        return r;
    }

    virtual ~reactor_server_t()
    {
        reactor.destroy( &reactor );
    }

private:
    us_allocator_t *allocator;
    us_reactor_t reactor;
};


}

#endif
