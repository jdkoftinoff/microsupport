#include "us_world.h"
#include "us_testutil.h"
#include "us_logger.h"
#include "us_logger_printer.h"
#include "us_allocator.h"
#include "us_net.h"
#include "us_slip.h"
#include "us_osc_msg.h"
#include "us_osc_msg_print.h"

/*
 Copyright (c) 2010, Meyer Sound Laboratories, Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of Meyer Sound Laboratories, Inc. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC.  BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

static bool us_tool_send_osc(
    const char *protocol,
    const char *src_host,
    const char *src_port,
    const char *dest_host,
    const char *dest_port,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
);


static us_osc_msg_t * us_tool_gen_osc(
    us_allocator_t *allocator,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
);

static bool us_tool_gen_flattened_osc(
    us_allocator_t *allocator,
    us_buffer_t *buffer,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
);

static bool us_tool_send_osc_udp(
    struct addrinfo *src_addr,
    struct addrinfo *dest_addr,
    us_buffer_t *buf
);

static bool us_tool_send_osc_tcp(
    struct addrinfo *dest_addr,
    us_buffer_t *buf
);

static bool us_tool_send_osc_tcpslip(
    struct addrinfo *dest_addr,
    us_buffer_t *buf
);

static us_osc_msg_t * us_tool_gen_osc(
    us_allocator_t *allocator,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
)
{
    us_osc_msg_t *r = 0;
    us_osc_msg_t *self;
    const char *t=osc_typetags;
    us_osc_msg_element_t *e;
    if ( *t==',' )
        ++t;
    self = us_osc_msg_create( allocator, osc_address );
    if ( self )
    {
        r = self;
        for ( ; *t!=0 && osc_values!=0 && *osc_values!=0; ++t )
        {
            e = 0;
            switch ( *t )
            {
            case 'a':
            {
                uint32_t high, low;
                uint64_t val = strtoull( *osc_values, 0, 10 );
                high = (uint32_t)((val>>32)&0xffffffff);
                low = (uint32_t)(val&0xffffffff);
                e = us_osc_msg_element_a_create( allocator, high, low );
                osc_values++;
            }
            break;
            case 'b':
            {
                /* TODO: */
                us_log_error( "'b' typetag not supported yet");
            }
            break;
#if US_ENABLE_DOUBLE
            case 'd':
            {
                double v;
                v = strtod( *osc_values, 0 );
                e = us_osc_msg_element_d_create( allocator, v );
                osc_values++;
            }
            break;
#endif
            case 'F':
            {
                e = us_osc_msg_element_F_create( allocator );
                osc_values++;
            }
            break;
#if US_ENABLE_FLOAT
            case 'f':
            {
                float v;
                v = strtof( *osc_values, 0 );
                e = us_osc_msg_element_f_create( allocator, v );
                osc_values++;
            }
            break;
#endif
            case 'h':
            {
                uint32_t high, low;
                uint64_t val = strtoull( *osc_values, 0, 10 );
                high = (uint32_t)((val>>32)&0xffffffff);
                low = (uint32_t)(val&0xffffffff);
                e = us_osc_msg_element_h_create( allocator, high, low );
                osc_values++;
            }
            break;
            case 'I':
            {
                e = us_osc_msg_element_I_create( allocator );
            }
            break;
            case 'i':
            {
                int32_t v;
                v = strtol( *osc_values, 0, 10 );
                e = us_osc_msg_element_i_create( allocator, v );
                osc_values++;
            }
            break;
            case 'N':
            {
                e = us_osc_msg_element_N_create( allocator );
            }
            break;
            case 's':
            {
                const char *s;
                s = *osc_values;
                e = us_osc_msg_element_s_create( allocator, s );
                osc_values++;
            }
            break;
            case 'T':
            {
                e = us_osc_msg_element_T_create( allocator );
            }
            break;
            case 't':
            {
                uint32_t high, low;
                uint64_t val = strtoull( *osc_values, 0, 10 );
                high = (uint32_t)((val>>32)&0xffffffff);
                low = (uint32_t)(val&0xffffffff);
                e = us_osc_msg_element_t_create( allocator, high, low );
                osc_values++;
            }
            break;
            default:
            {
                e = 0;
            }
            break;
            }
            if ( e )
            {
                self->append( self, e );
            }
            else
            {
                r = 0;
                break;
            }
        }
    }
    return r;
}

static bool us_tool_gen_flattened_osc(
    us_allocator_t *allocator,
    us_buffer_t *buffer,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
)
{
    bool r=false;
    us_osc_msg_t *msg = us_tool_gen_osc(allocator, osc_address, osc_typetags, osc_values);
    if( msg )
    {
        r=msg->flatten( msg, buffer, 0 );
        msg->destroy( msg );
    }
    return r;
}

static bool us_tool_send_osc_udp(
    struct addrinfo *src_addr,
    struct addrinfo *dest_addr,
    us_buffer_t *buf
)
{
    bool r=false;
    int s = socket( src_addr->ai_family, src_addr->ai_socktype, 0 );
    if( s>=0 )
    {
        if( sendto(s, buf->m_buffer, buf->m_cur_length, 0, dest_addr->ai_addr, dest_addr->ai_addrlen)==buf->m_cur_length )
        {
            r=true;
        }
        else
        {
            us_stderr->printf( us_stderr, "sendto: %s\n", strerror(errno));
        }
        closesocket(s);
    }
    return r;
}

static bool us_tool_send_osc_tcp(
    struct addrinfo *dest_addr,
    us_buffer_t *buf
)
{
    bool r=false;
    int s = us_net_create_tcp_socket(dest_addr, false);
    if( s>=0 )
    {
        if( connect(s, dest_addr->ai_addr, dest_addr->ai_addrlen)>=0 )
        {
            uint8_t length[4];
            r=true;
            length[0] = US_GET_BYTE_3( buf->m_cur_length );
            length[1] = US_GET_BYTE_2( buf->m_cur_length );
            length[2] = US_GET_BYTE_1( buf->m_cur_length );
            length[3] = US_GET_BYTE_0( buf->m_cur_length );
            r&=us_net_blocking_send(s, length, sizeof(length) );
            if( r )
            {
                r&=us_net_blocking_send(s, buf->m_buffer, buf->m_cur_length);
            }
            if( !r )
            {
                us_stderr->printf( us_stderr, "Error sending to TCP socket: %s\n", strerror(errno) );
            }
        }
        else
        {
            r=false;
            us_stderr->printf( us_stderr, "Error connecting: %s\n", strerror(errno) );
        }
        closesocket(s);
    }
    return r;
}

static bool us_tool_send_osc_tcpslip(
    struct addrinfo *dest_addr,
    us_buffer_t *buf
)
{
    bool r=false;
    int s;
    uint8_t slipped_mem[4096];
    us_buffer_t slipped_buffer;
    us_buffer_init(&slipped_buffer, 0, slipped_mem, sizeof( slipped_mem ) );
    us_slip_encode(&slipped_buffer, buf);
    s = us_net_create_tcp_socket(dest_addr, false);
    if( s>=0 )
    {
        if( connect(s, dest_addr->ai_addr, dest_addr->ai_addrlen)>=0 )
        {
            r=true;
            r&=us_net_blocking_send(s, slipped_buffer.m_buffer, slipped_buffer.m_cur_length);
            if( !r )
            {
                us_stderr->printf( us_stderr, "Error sending slipped packet to TCP socket: %s\n", strerror(errno) );
            }
        }
        else
        {
            r=false;
            us_stderr->printf( us_stderr, "Error connecting: %s\n", strerror(errno) );
        }
        closesocket(s);
    }
    return r;
}


static bool us_tool_send_osc(
    const char *protocol,
    const char *src_host,
    const char *src_port,
    const char *dest_host,
    const char *dest_port,
    const char *osc_address,
    const char *osc_typetags,
    char **osc_values
)
{
    bool r=false;
    char **v;
    const char *t=osc_typetags;
    us_allocator_t *allocator = us_testutil_sys_allocator;
    int sock_type=SOCK_DGRAM;
    struct addrinfo *src_addr;
    struct addrinfo *dest_addr;
    if( strcmp( protocol, "udp" )==0 )
    {
        sock_type = SOCK_DGRAM;
    }
    else if (strcmp( protocol, "tcp" )==0 || strcmp( protocol, "tcpslip" )==0 )
    {
        sock_type = SOCK_STREAM;
    }
    else
    {
        us_stderr->printf( us_stderr, "unknown protocol '%s', use 'tcp', 'tcpslip' or 'udp'\n", protocol );
        return false;
    }
    src_addr = us_net_get_addrinfo(src_host, src_port, sock_type, true);
    if( !src_addr )
    {
        us_stderr->printf( us_stderr, "unable to get src address for '[%s]:%s'\n", dest_host, dest_port );
        return false;
    }
    dest_addr = us_net_get_addrinfo(dest_host, dest_port, sock_type, false);
    if( !dest_addr )
    {
        us_stderr->printf( us_stderr, "unable to get dest address for '[%s]:%s'\n", dest_host, dest_port );
        freeaddrinfo(src_addr);
        return false;
    }
    us_buffer_t *buf = us_buffer_create(allocator, 4096);
    if( buf )
    {
        us_log_info( "Sending '%s' to '[%s]:%s' osc address '%s' types ',%s' ", protocol, dest_host, dest_port, osc_address, osc_typetags );
        if( osc_values )
        {
            for( v=osc_values; *v!=0 && t!=0; v++,t++ )
            {
                us_log_info( "Type '%c' Value: '%s'", *t, *v );
            }
        }
        r=us_tool_gen_flattened_osc( us_testutil_sys_allocator, buf, osc_address, osc_typetags, osc_values );
        if( r )
        {
            buf->print( buf, us_stdout );
            if( sock_type==SOCK_DGRAM )
            {
                r=us_tool_send_osc_udp( src_addr, dest_addr, buf );
            }
            else if( sock_type == SOCK_STREAM )
            {
                if( strcmp(protocol, "tcpslip" )==0 )
                {
                    r=us_tool_send_osc_tcpslip( dest_addr, buf );
                }
                else
                {
                    r=us_tool_send_osc_tcp(dest_addr, buf);
                }
            }
        }
        buf->destroy( buf );
    }
    freeaddrinfo(dest_addr);
    return r;
}


int main ( int argc, char **argv )
{
    int r = 1;
    const char *protocol = "udp";
    const char *src_host = "";
    const char *src_port = "30000";
    const char *dest_host = "127.0.0.1";
    const char *dest_port = "10000";
    const char *osc_address = "/osc/version";
    const char *osc_typetags = "";
    char **osc_values = 0;
    if( argc > 1 )
        protocol = argv[1];
    if ( argc > 2 )
        src_host = argv[2];
    if ( argc > 3 )
        src_port = argv[3];
    if ( argc > 4 )
        dest_host = argv[4];
    if ( argc > 5 )
        dest_port = argv[5];
    if ( argc > 6 )
        osc_address = argv[6];
    if ( argc > 7 )
        osc_typetags = argv[7];
    if ( argc > 8 )
        osc_values = &argv[8];
    if ( us_testutil_start ( 8192, 8192, argc, argv ) )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start ( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
# ifdef WIN32
        us_platform_init_winsock ();
# endif
        us_log_set_level ( US_LOG_LEVEL_DEBUG );
        if ( argc < 7 )
        {
            us_stderr->printf( us_stderr, "%s usage:\n\t%s [udp/tcp/tcpslip] [src host] [src port] [dest host] [dest port] [osc address] [osc typetags] {values...}\n", argv[0], argv[0] );
        }
        else
        {
            if(  us_tool_send_osc( protocol, src_host, src_port, dest_host, dest_port, osc_address, osc_typetags, osc_values ) )
                r=0;
        }
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}

