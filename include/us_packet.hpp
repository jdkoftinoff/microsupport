#ifndef US_PACKET_HPP
#define US_PACKET_HPP

/*
 Copyright (c) 2012, Meyer Sound Laboratories, Inc.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the Meyer Sound Laboratories, Inc. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

#include "us_world.hpp"

#include "us_packet.h"

    /* \addtogroup packet packet*/
    /*@{*/

namespace microsupport
{
    struct packet_address_t : us_packet_address_t
    {
        const packet_address_t & operator = ( const us_packet_address_t &v )
        {
            m_type = v.m_type;
            if( v.m_type == us_packet_address_mac48 )
            {
                mac48.m_if_id = v.mac48.m_if_id;
                memcpy( mac48.m_mac, v.mac48.m_mac, sizeof( mac48.m_mac));
            }
            else if( v.m_type==us_packet_address_tcp )
            {
                tcp.m_len = v.tcp.m_len;
                tcp.m_addr = v.tcp.m_addr;
            }

            return *this;
        }

        packet_address_t( const us_packet_address_t &v )
        {
            m_type = v.m_type;
            if( v.m_type == us_packet_address_mac48 )
            {
                mac48.m_if_id = v.mac48.m_if_id;
                memcpy( mac48.m_mac, v.mac48.m_mac, sizeof( mac48.m_mac));
            }
            else if( v.m_type==us_packet_address_tcp )
            {
                tcp.m_len = v.tcp.m_len;
                tcp.m_addr = v.tcp.m_addr;
            }
        }
        packet_address_t( const char *ifname, uint64_t mac )
        {
            m_type = us_packet_address_mac48;
            mac48.m_if_id = if_nametoindex( ifname );
            mac48.m_mac[0] = US_GET_BYTE_5( mac );
            mac48.m_mac[1] = US_GET_BYTE_4( mac );
            mac48.m_mac[2] = US_GET_BYTE_3( mac );
            mac48.m_mac[3] = US_GET_BYTE_2( mac );
            mac48.m_mac[4] = US_GET_BYTE_1( mac );
            mac48.m_mac[5] = US_GET_BYTE_0( mac );
        }

        packet_address_t( int if_id, const uint8_t mac[6] )
        {
            m_type = us_packet_address_mac48;
            mac48.m_if_id = if_id;
            memcpy( mac48.m_mac, mac, sizeof( mac48.m_mac ));
        }

        packet_address_t( int if_id, const struct sockaddr *addr, size_t addrlen )
        {
            m_type = us_packet_address_tcp;
            tcp.m_if_id = if_id;
            memcpy( &tcp.m_addr, addr, addrlen );
        }

        std::ostream & print( std::ostream &s ) const
        {
            bool r;
            char buf[8192];
            size_t offset = 0;
            r = print( buf,&offset,sizeof(buf) );
            if( r )
            {
                s << buf;
            }
            else
            {
                s << "None";
            }
            return s;
        }

        bool print( char *buf, size_t *pos, size_t len ) const
        {
            bool r=false;
            if( m_type == us_packet_address_mac48 )
            {
                int cnt;
                cnt=snprintf( (buf+*pos), len-*pos, "%s if:%d %02x-%02x-%02x-%02x-%02x-%02x",
                              "MAC48",
                              mac48.m_if_id,
                              mac48.m_mac[0],
                              mac48.m_mac[1],
                              mac48.m_mac[2],
                              mac48.m_mac[3],
                              mac48.m_mac[4],
                              mac48.m_mac[5]
                              );
                if( cnt>0 )
                {
                    *pos += cnt;
                }
                r = (cnt>0);
            }
            else if( m_type == us_packet_address_tcp )
            {
                char hostbuf[512];
                char portbuf[64];

                if( getnameinfo(
                            (const sockaddr *)&tcp.m_addr,
                            tcp.m_len,
                            hostbuf, sizeof(hostbuf),
                            portbuf, sizeof(portbuf),
                            NI_NUMERICHOST | NI_NUMERICSERV ) ==0 )
                {
                    int cnt;
                    cnt=snprintf( (buf+*pos), len-*pos, "%s if:%d %s:%s",
                                  "TCP",
                                  tcp.m_if_id,
                                  hostbuf,
                                  portbuf
                                  );
                    if( cnt>0 )
                    {
                        *pos += cnt;
                    }
                    r=true;
                }
            }
            return r;
        }
    };

    inline std::ostream &operator << (std::ostream &s, const packet_address_t &v )
    {
        return v.print( s );
    }



    bool operator == (
                const us_packet_address_mac48_t &left,
                const us_packet_address_mac48_t &right
                );

    bool operator < (
                const us_packet_address_mac48_t &left,
                const us_packet_address_mac48_t &right
                );

    bool operator == (
                const us_packet_address_tcp_t &left,
                const us_packet_address_tcp_t &right
                );

    bool operator < (
                const us_packet_address_tcp_t &left,
                const us_packet_address_tcp_t &right
                );

    bool operator == (
                const packet_address_t &left,
                const packet_address_t &right
                );

    bool operator < (
                const packet_address_t &left,
                const packet_address_t &right
                );

    class packet_t : public us_packet_t
    {
    public:
        std::ostream &print(std::ostream &s) const
        {
            s << "src_address: " << *src_address() << std::endl;
            s << "dest_address: " << *dest_address() << std::endl;
            s << "length: " << length() << std::endl;
            s << "max_length: " << max_length() << std::endl;
            s << "data:" << std::endl;
            const uint8_t *d = data();
            for( size_t i=0; i<length(); ++i )
            {
                char buf[8];
                sprintf( buf, "%02x ", d[i] );
                s << buf;
            }
            s << std::endl;
            return s;
        }

        void clear()
        {
            us_packet_clear(this);
        }
        const uint8_t *data() const
        {
            return us_packet_get_data(this);
        }
        uint8_t *data()
        {
            return us_packet_data(this);
        }
        size_t max_length() const
        {
            return us_packet_get_max_length(this);
        }
        size_t length() const
        {
            return us_packet_get_length(this);
        }
        void length( size_t n )
        {
            us_packet_set_length( this, n );
        }
        const packet_address_t *src_address() const
        {
            return reinterpret_cast<const packet_address_t *>( &m_src_address );
        }
        const packet_address_t *dest_address() const
        {
            return reinterpret_cast<const packet_address_t *>( &m_dest_address );
        }
    };

    inline std::ostream &operator << (std::ostream &s, const packet_t &v )
    {
        return v.print( s );
    }

}


/*@}*/

#endif

