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
    
inline bool operator == ( 
                         const us_packet_address_mac48_t &left,
                         const us_packet_address_mac48_t &right
                         )
{
    bool r=true;
    for( int i=0; i<6; ++i )
    {
        if( left.m_mac[i] != right.m_mac[i] )
        {
            r=false;
            break;
        }
        if( r )
        {
            if( left.m_if_id != right.m_if_id )
            {
                r=false;
            }
        }
    }
        
    return r;
}

inline bool operator < (
                        const us_packet_address_mac48_t &left,
                        const us_packet_address_mac48_t &right
                        )
{
    bool r=false;
    
    if( left.m_if_id < right.m_if_id )
    {
        r=true;
    }
    else
    {
        for( int i=0; i<6; ++i )
        {
            if( left.m_mac[i] < right.m_mac[i] )
            {
                r=true;
                break;
            }
        }
    }
    
    return r;
}
                        
inline bool operator == ( 
                         const us_packet_address_tcp_t &left,
                         const us_packet_address_tcp_t &right 
                         )
{
    bool r=true;
    if( left.m_if_id != right.m_if_id )
    {
        r=false;
    }
    else
    {
        if( left.m_addr.ss_family != right.m_addr.ss_family )
        {
            r=false;
        }
        else
        {
            if( left.m_addr.ss_family == AF_UNIX )
            {
            }
            else if( left.m_addr.ss_family == AF_INET )
            {
                const struct sockaddr_in *pleft = (const struct sockaddr_in *)&left.m_addr;
                const struct sockaddr_in *pright = (const struct sockaddr_in *)&right.m_addr;
                if( pleft->sin_port != pright->sin_port )
                {
                    r=false;
                }
                else
                {
                    if( memcmp( &pleft->sin_addr, &pright->sin_addr, sizeof( struct in_addr ) ) != 0 )
                    {
                        r=false;
                    }
                }                
            }
            else if( left.m_addr.ss_family == AF_INET6 )
            {
                const struct sockaddr_in6 *pleft = (const struct sockaddr_in6 *)&left.m_addr;
                const struct sockaddr_in6 *pright = (const struct sockaddr_in6 *)&right.m_addr;
                if( pleft->sin6_port != pright->sin6_port 
                   || pleft->sin6_scope_id != pright->sin6_scope_id )
                {
                    r=false;
                }
                else
                {
                    if( memcmp( &pleft->sin6_addr, &pright->sin6_addr, sizeof( struct in6_addr ) ) != 0 )
                    {
                        r=false;
                    }
                }                
                
            }
            else
            {
                r=false;
            }
            
        }
    }
    return r;
}
    
inline bool operator < ( 
                         const us_packet_address_tcp_t &left,
                         const us_packet_address_tcp_t &right 
                         )
{
    bool r=false;
    if( left.m_if_id < right.m_if_id )
    {
        r=true;
    }
    else
    {
        if( left.m_addr.ss_family < right.m_addr.ss_family )
        {
            r=true;
        }
        else
        {
            if( left.m_addr.ss_family == AF_UNIX )
            {
            }
            else if( left.m_addr.ss_family == AF_INET )
            {
                const struct sockaddr_in *pleft = (const struct sockaddr_in *)&left.m_addr;
                const struct sockaddr_in *pright = (const struct sockaddr_in *)&right.m_addr;
                if( pleft->sin_port < pright->sin_port )
                {
                    r=true;
                }
                else
                {
                    if( memcmp( &pleft->sin_addr, &pright->sin_addr, sizeof( struct in_addr ) ) < 0 )
                    {
                        r=true;
                    }
                }                
            }
            else if( left.m_addr.ss_family == AF_INET6 )
            {
                const struct sockaddr_in6 *pleft = (const struct sockaddr_in6 *)&left.m_addr;
                const struct sockaddr_in6 *pright = (const struct sockaddr_in6 *)&right.m_addr;
                if( pleft->sin6_port < pright->sin6_port
                   || pleft->sin6_scope_id < pright->sin6_scope_id )
                {
                    r=true;
                }
                else
                {
                    if( memcmp( &pleft->sin6_addr, &pright->sin6_addr, sizeof( struct in6_addr ) ) < 0 )
                    {
                        r=true;
                    }
                }                
                
            }
            else
            {
                r=true;
            }
            
        }
    }
    return r;
}

inline bool operator == ( const us_packet_address_t &left, const us_packet_address_t &right )
{
    bool r=false;
    if( left.m_type == right.m_type )
    {
        switch( left.m_type )
        {
        case us_packet_address_none:
            r=true;
            break;
        case us_packet_address_mac48:
            r= (left.address.mac48 == right.address.mac48);
            break;
        case us_packet_address_tcp:
            r= (left.address.tcp == right.address.tcp);
            break;
        default:
            r=false;
            break;
        }
    }
    return r;
}

inline bool operator < ( const us_packet_address_t &left, const us_packet_address_t &right )
{
    bool r=false;
    if( left.m_type == right.m_type )
    {
        switch( left.m_type )
        {
        case us_packet_address_none:
            r=false;
            break;
        case us_packet_address_mac48:
            r= (left.address.mac48 < right.address.mac48);
            break;
        case us_packet_address_tcp:
            r= (left.address.tcp < right.address.tcp);
            break;
        default:
            r=false;
            break;
        }
    }
    else
    {
        r=( (int)left.m_type < (int)right.m_type);
    }
    return r;
}

/*@}*/

#endif

