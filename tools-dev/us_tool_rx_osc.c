#include "us_world.h"
#include "us_logger.h"
#include "us_logger_printer.h"
#include "us_net.h"
#include "us_reactor.h"
#include "us_osc_msg.h"

/**
 \addtogroup us_tool_rx_osc
 @{
 */

void us_tool_rx_osc_udp_packet_received(
    us_reactor_handler_udp_t *self,
    us_buffer_t *buf,
    struct sockaddr *remote_addr,
    socklen_t remote_addrlen
)
{
    us_osc_msg_t *msg = us_osc_msg_unflatten(self->m_base.m_allocator, buf );
    if( !msg )
    {
        us_log_error( "unable to parse osc message" );
    }
    else
    {
        us_log_debug( "parsed osc msg" );
        msg->print( msg, us_stdout );
        us_stdout->printf( us_stdout, "\n" );
        //received_osc( self, msg );
        msg->destroy( msg );
    }
}

bool us_tool_rx_osc_udp_init( us_reactor_handler_t *self_, us_allocator_t *allocator, int fd, void *extra )
{
    bool r=true;
    r&=us_reactor_handler_udp_init(self_, allocator, fd, extra, 8192);
    if( r )
    {
        us_reactor_handler_udp_t *self = (us_reactor_handler_udp_t *)self_;
        self->packet_received = us_tool_rx_osc_udp_packet_received;
        self_->m_wake_on_readable = true;
    }
    return r;
}

typedef struct us_tool_rx_osc_tcp_handler_s
{
    us_reactor_handler_tcp_t m_base;
    bool m_in_header;
    uint32_t m_todo_count;
    void (*received_osc)( struct us_tool_rx_osc_tcp_handler_s *self, const us_osc_msg_t *msg );
} us_tool_rx_osc_tcp_handler_t;

bool us_tool_rx_osc_tcp_handler_readable (
    us_reactor_handler_tcp_t *self_
)
{
    bool done=false;
    us_tool_rx_osc_tcp_handler_t *self = ( us_tool_rx_osc_tcp_handler_t * ) self_;
    us_queue_t *incoming = &self->m_base.m_incoming_queue;
    while (!done && us_queue_readable_count(incoming)>0 )
    {
        if( self->m_in_header  )
        {
            if( us_queue_readable_count( incoming )>=4 )
            {
                self->m_todo_count =
                    (((int32_t)us_queue_read_byte( incoming )) << 24) +
                    (((int32_t)us_queue_read_byte( incoming )) << 16) +
                    (((int32_t)us_queue_read_byte( incoming )) << 8)+
                    (((int32_t)us_queue_read_byte( incoming )) << 0);
                self->m_in_header = false;
                us_log_debug( "got osc length field: %d", self->m_todo_count );
            }
            if( self->m_in_header == true )
            {
                done=true;
            }
        }
        if( !self->m_in_header )
        {
            if( us_queue_readable_count( incoming )>=self->m_todo_count )
            {
                uint8_t flattened[4096];
                int p=0;
                while ( us_queue_can_read_byte ( incoming ) )
                {
                    if( p<sizeof(flattened) )
                    {
                        flattened[p] = us_queue_read_byte(incoming);
                    }
                    if( ++p == self->m_todo_count )
                    {
                        us_buffer_t buf;
                        us_osc_msg_t *msg;
                        us_buffer_init(&buf, 0, flattened, p);
                        buf.m_cur_length = p;
                        buf.m_cur_read_pos = 0;
                        us_log_debug( "pulled in raw osc msg of length %d", p );
                        msg = us_osc_msg_unflatten(self->m_base.m_base.m_allocator, &buf );
                        if( !msg )
                        {
                            us_log_error( "unable to parse osc message" );
                            break;
                        }
                        else
                        {
                            us_log_debug( "parsed osc msg" );
                            self->m_in_header = true;
                            msg->print( msg, us_stdout );
                            us_stdout->printf( us_stdout, "\n" );
                            if( self->received_osc )
                            {
                                self->received_osc( self, msg );
                            }
                            msg->destroy( msg );
                        }
                    }
                }
                if( self->m_in_header == false )
                {
                    done=true;
                }
            }
        }
    }
    return true;
}

us_reactor_handler_t *
us_tool_rx_osc_tcp_handler_create ( us_allocator_t *allocator )
{
    return (us_reactor_handler_t *)us_new( allocator, us_tool_rx_osc_tcp_handler_t );
}

bool us_tool_rx_osc_tcp_handler_init (
    us_reactor_handler_t *self_,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    us_tool_rx_osc_tcp_handler_t *self = ( us_tool_rx_osc_tcp_handler_t * ) self_;
    bool r;
    r = us_reactor_handler_tcp_init (
            self_,
            allocator,
            fd,
            extra,
            16384,
            1500
        );
    self->m_base.readable = us_tool_rx_osc_tcp_handler_readable;
    self->m_base.tick = 0;
    self->m_in_header = true;
    self->m_todo_count = 0;
    self->received_osc = 0;
    return r;
}


bool us_tool_rx_osc_tcp_server_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra
)
{
    return us_reactor_handler_tcp_server_init (
               self,
               allocator,
               fd,
               extra,
               us_tool_rx_osc_tcp_handler_create,
               us_tool_rx_osc_tcp_handler_init
           );
}

static bool us_tool_rx_osc( us_allocator_t *allocator, const char *listen_host, const char *listen_port )
{
    bool r = false;
    us_reactor_t reactor;
    r = us_reactor_init (
            &reactor,
            allocator,
            16 /* max simultaneous sockets, including server sockets and connections */
        );
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                allocator,
                listen_host, listen_port,
                SOCK_STREAM,
                0,
                us_reactor_handler_tcp_server_create,
                us_tool_rx_osc_tcp_server_init
            );
    }
    if ( r )
    {
        r = us_reactor_create_server (
                &reactor,
                allocator,
                listen_host, listen_port,
                SOCK_DGRAM,
                0,
                us_reactor_handler_udp_create,
                us_tool_rx_osc_udp_init
            );
    }
    if ( r )
    {
        while ( reactor.poll ( &reactor, 2000 ) )
        {
            fprintf ( stdout, "tick\n" );
        }
        reactor.destroy ( &reactor );
        r = true;
    }
    return r;
}

int main ( int argc, char **argv )
{
    int r = 1;
    us_print_file_t us_stdout_printer;
    us_print_file_t us_stderr_printer;
    const char * listen_host = "127.0.0.1";
    const char * listen_port = "10000";
    us_malloc_allocator_t allocator;
    us_malloc_allocator_init( &allocator );
    if ( (argc == 2 && strcmp ( "--help", argv[1] ) == 0) || argc==1 )
    {
        fprintf ( stdout, "usage:\n\t%s {listen host} {listen port}\ndefaults to: %s %s\n",
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
    us_stdout =
        us_print_file_init (
            &us_stdout_printer,
            stdout
        );
    us_stderr =
        us_print_file_init (
            &us_stderr_printer,
            stderr
        );
    us_logger_printer_start ( us_stdout, us_stderr );
# ifdef WIN32
    us_platform_init_winsock ();
# endif
    us_log_set_level ( US_LOG_LEVEL_DEBUG );
    if( us_tool_rx_osc(&allocator.m_base, listen_host, listen_port) )
    {
        r=0;
    }
    us_logger_finish();
    us_malloc_allocator_destroy( &allocator.m_base );
    return r;
}

/*@}*/
