#ifndef US_PACKET_H
#define US_PACKET_H


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

#include "us_world.h"
#include "us_allocator.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* \addtogroup packet packet*/
/*@{*/

typedef enum us_packet_address_type_e
{
    us_packet_address_none = 0,
    us_packet_address_mac48,
    us_packet_address_tcp
}
us_packet_address_type_t;

typedef struct us_packet_address_mac48_s
{
    uint8_t m_mac[6];
    int m_if_id;
} us_packet_address_mac48_t;

typedef struct us_packet_address_tcp_s
{
    size_t m_len;
    struct sockaddr_storage m_addr;
    int m_if_id;
} us_packet_address_tcp_t;

typedef struct us_packet_address_s
{
    us_packet_address_type_t m_type;
    us_packet_address_mac48_t mac48;
    us_packet_address_tcp_t tcp;
} us_packet_address_t;

typedef struct us_packet_s
{
    us_allocator_t *m_allocator;
    uint8_t *m_data;
    size_t m_max_length;
    size_t m_length;
    us_packet_address_t m_src_address;
    us_packet_address_t m_dest_address;
    void (*destroy)( struct us_packet_s *self );
} us_packet_t;


us_packet_t * us_packet_create( us_allocator_t *allocator, size_t max_size );
static inline void us_packet_clear( us_packet_t *self )
{
    self->m_length = 0;
    self->m_src_address.m_type = us_packet_address_none;
    self->m_dest_address.m_type = us_packet_address_none;
}
void us_packet_destroy( us_packet_t *self );


static inline uint8_t *us_packet_data( us_packet_t *self )
{
    return self->m_data;
}

static inline const uint8_t *us_packet_get_data( const us_packet_t *self )
{
    return self->m_data;
}

static inline size_t us_packet_get_max_length( const us_packet_t *self )
{
    return self->m_max_length;
}

static inline size_t us_packet_get_length( const us_packet_t *self )
{
    return self->m_length;
}

static inline bool us_packet_set_length( us_packet_t *self, size_t new_length )
{
    bool r=false;
    if( new_length <= self->m_max_length)
    {
        self->m_length = new_length;
        r=true;
    }
    return r;
}

static inline bool us_packet_copy(
    us_packet_t *self,
    const uint8_t *raw_data,
    size_t raw_data_length,
    const us_packet_address_t *src_address,
    const us_packet_address_t *dest_address
    )
{
    bool r=false;
    if( raw_data_length <= self->m_max_length )
    {
        memcpy( self->m_data, raw_data, raw_data_length );
        self->m_length = raw_data_length;
        if( src_address )
        {
            self->m_src_address = *src_address;
        }
        else
        {
            self->m_src_address.m_type = us_packet_address_none;
        }
        if( dest_address )
        {
            self->m_dest_address = *dest_address;
        }
        else
        {
            self->m_dest_address.m_type = us_packet_address_none;
        }
        r=true;
    }
    return r;
}


/*@}*/

#ifdef __cplusplus
}
#endif


#endif

