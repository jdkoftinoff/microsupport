#ifndef US_BITS_H
#define US_BITS_H

/*
Copyright (c) 2011, Meyer Sound Laboratories, Inc.
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

#ifndef US_WORLD_H
#include "us_world.h"
#endif

/**
 \addtogroup us_bits IEEE bit packing utilities
 */

/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif



/** \addtogtoup bit_util bit utilities

/*@{*/

#define US_OCTET_BIT(B) (((uint8_t)0x80)>>(B))
#define US_DOUBLET_BIT(B) (((uint16_t)0x8000)>>(B))      
#define US_QUADLET_BIT(B) (((uint32_t)0x80000000)>>(B)) 

#define US_OCTET_MASK_BIT(B) ((~US_OCTET_BIT(B))&0xff)
#define US_DOUBLET_MASK_BIT(B) ((~US_DOUBLET_BIT(B))&0xffff)
#define US_QUADLET_MASK_BIT(B) ((~US_QUADLET_BIT(B))&0xffffffff)


#define US_IS_BITVALUE_SET( VALUE, BIT_VALUE ) (((VALUE)&(BIT_VALUE))!=0)
#define US_IS_BITVALUE_CLEAR( VALUE, BIT_VALUE ) (((VALUE)&(BIT_VALUE))==0)

#define US_IS_OCTET_BIT_SET( VALUE, BIT ) US_IS_BITVALUE_SET(VALUE,US_OCTET_BIT(BIT))
#define US_IS_OCTET_BIT_CLEAR( VALUE, BIT ) US_IS_BITVALUE_CLEAR(VALUE,US_OCTET_BIT(BIT))

#define US_IS_DOUBLET_BIT_SET( VALUE, BIT ) US_IS_BITVALUE_SET(VALUE,US_DOUBLET_BIT(BIT))
#define US_IS_DOUBLET_BIT_CLEAR( VALUE, BIT ) US_IS_BITVALUE_CLEAR(VALUE,US_DOUBLET_BIT(BIT))

#define US_IS_QUADLET_BIT_SET( VALUE, BIT ) US_IS_BITVALUE_SET(VALUE,US_QUADLET_BIT(BIT))
#define US_IS_QUADLET_BIT_CLEAR( VALUE, BIT ) US_IS_BITVALUE_CLEAR(VALUE,US_QUADLET_BIT(BIT))

#define US_SET_OCTET_BIT( VAL, B )  ((VAL) | US_OCTET_BIT(B))
#define US_CLEAR_OCTET_BIT( VAL, B ) ((VAL) & US_OCTET_MASK_BIT(B))

#define US_SET_DOUBLET_BIT( VAL, B )  ((VAL) | US_DOUBLET_BIT(B))
#define US_CLEAR_DOUBLET_BIT( VAL, B ) ((VAL) & US_DOUBLET_MASK_BIT(B))

#define US_SET_QUADLET_BIT( VAL, B )  ((VAL) | US_QUADLET_BIT(B))
#define US_CLEAR_QUADLET_BIT( VAL, B ) ((VAL) & US_QUADLET_MASK_BIT(B))

#define US_SET_OCTET_BIT_IF( VAL, B, Q )  ((Q) ? US_SET_OCTET_BIT(VAL,B) : US_CLEAR_OCTET_BIT(VAL,B))
#define US_CLEAR_OCTET_BIT_IF( VAL, B, Q )  ((!Q) ? US_SET_OCTET_BIT(VAL,B) : US_CLEAR_OCTET_BIT(VAL,B))

#define US_SET_DOUBLET_BIT_IF( VAL, B, Q )  ((Q) ? US_SET_DOUBLET_BIT(VAL,B) : US_CLEAR_DOUBLET_BIT(VAL,B))
#define US_CLEAR_DOUBLET_BIT_IF( VAL, B, Q )  ((!Q) ? US_SET_DOUBLET_BIT(VAL,B) : US_CLEAR_DOUBLET_BIT(VAL,B))

#define US_SET_QUADLET_BIT_IF( VAL, B, Q )  ((Q) ? US_SET_QUADLET_BIT(VAL,B) : US_CLEAR_QUADLET_BIT(VAL,B))
#define US_CLEAR_QUADLET_BIT_IF( VAL, B, Q )  ((!Q) ? US_SET_QUADLET_BIT(VAL,B) : US_CLEAR_QUADLET_BIT(VAL,B))

/*@}*/

/** \addtogtoup bitfield bitfield utility functions. BIT is IEEE style bit number of the lsb of the field */

/*@{*/

#define US_OCTET_BITFIELD(BIT,WIDTH) (((0xff)>>(8-(WIDTH)))<<(7-(BIT)))
#define US_DOUBLET_BITFIELD(BIT,WIDTH) (((0xffff)>>(16-(WIDTH)))<<(15-(BIT)))
#define US_QUADLET_BITFIELD(BIT,WIDTH) (((0xffffffff)>>(32-(WIDTH)))<<(31-(BIT)))

#define US_OCTET_MASK_BITFIELD(BIT,WIDTH) ((~US_OCTET_BITFIELD(BIT,WIDTH))&0xff)
#define US_DOUBLET_MASK_BITFIELD(BIT,WIDTH) ((~US_DOUBLET_BITFIELD(BIT,WIDTH))&0xffff)
#define US_QUADLET_MASK_BITFIELD(BIT,WIDTH) ((~ADECC_QUADLET_BITFIELD(BIT,WIDTH))&0xffffffff)

#define US_GET_OCTET_BITFIELD(VAL,BIT,WIDTH)  (((VAL) & US_OCTET_BITFIELD(BIT,BITCOUNT)) >> (7-(BIT)))
#define US_SET_OCTET_BITFIELD(VAL,BIT,WIDTH,FIELDVAL) (((FIELDVAL)<<(7-(BIT))) | ((VAL) & US_OCTET_MASK_BITFIELD(BIT,BITCOUNT)))

#define US_GET_DOUBLET_BITFIELD(VAL,BIT,WIDTH)  (((VAL) & US_DOUBLET_BITFIELD(BIT,BITCOUNT)) >> (15-(BIT)))
#define US_SET_DOUBLET_BITFIELD(VAL,BIT,WIDTH,FIELDVAL) (((FIELDVAL)<<(15-(BIT))) | ((VAL) & US_DOUBLET_MASK_BITFIELD(BIT,BITCOUNT)))

#define US_GET_QUADLET_BITFIELD(VAL,BIT,WIDTH)  (((VAL) & US_DOUBLET_BITFIELD(BIT,BITCOUNT)) >> (31-(BIT)))
#define US_SET_QUADLET_BITFIELD(VAL,BIT,WIDTH,FIELDVAL) (((FIELDVAL)<<(31-(BIT))) | ((VAL) & US_DOUBLET_MASK_BITFIELD(BIT,BITCOUNT)))

/*@}*/

#define US_MAKE_UINT16( B1, B0 ) ( (((uint16_t)(B1))<<8) | (((uint16_t)(B0))<<0) )

#define US_MAKE_UINT32( B3, B2, B1, B0 ) ( (((uint32_t)(B3))<<24) | (((uint32_t)(B2))<<16) | (((uint32_t)(B1))<<8) | (((uint32_t)(B0))<<0) )

#define US_MAKE_UINT64( B7, B6, B5, B4, B3, B2, B1, B0 ) \
    (((uint64_t)US_MAKE_UINT32( B7, B6, B5, B4 )<<32) | ((uint64_t)US_MAKE_UINT32(B3,B2,B1,B0)))


static inline
uint32_t us_extract_uint16 ( const void *base, uint16_t offset )
{
    const uint8_t *data = ( const uint8_t * ) base;
    return US_MAKE_UINT16 ( data[offset], data[offset+1] );
}

static inline
void us_store_uint16 ( void *base, uint16_t value, uint16_t offset )
{
    uint8_t *data = ( uint8_t * ) base;
    data[offset] = ( uint8_t ) ( value>>8 );
    data[offset+1] = ( uint8_t ) ( value );
}

static inline
uint32_t us_extract_uint32 ( const void *base, uint16_t offset )
{
    const uint8_t *data = ( const uint8_t * ) base;
    return US_MAKE_UINT32 ( data[offset], data[offset+1], data[offset+2], data[offset+3] );
}

static inline
void us_store_uint32 ( void *base, uint32_t value, uint16_t offset )
{
    uint8_t *data = ( uint8_t * ) base;
    data[offset] = ( uint8_t ) ( value>>24 );
    data[offset+1] = ( uint8_t ) ( value>>16 );
    data[offset+2] = ( uint8_t ) ( value>>8 );
    data[offset+3] = ( uint8_t ) ( value>>0 );
}

static inline
uint64_t us_extract_uint64 ( const void *base, uint16_t offset )
{
    const uint8_t *data = ( const uint8_t * ) base;
    return US_MAKE_UINT64 (
               data[offset],
               data[offset+1],
               data[offset+2],
               data[offset+3],
               data[offset+4],
               data[offset+5],
               data[offset+6],
               data[offset+7]
           );
}

static inline
void us_store_uint64 ( void *base, uint64_t value, uint16_t offset )
{
    uint8_t *data = ( uint8_t * ) base;
    data[offset] = ( uint8_t ) ( value>> ( 24+32 ) );
    data[offset+1] = ( uint8_t ) ( value>> ( 16+32 ) );
    data[offset+2] = ( uint8_t ) ( value>> ( 8+32 ) );
    data[offset+3] = ( uint8_t ) ( value>> ( 0+32 ) );
    data[offset+4] = ( uint8_t ) ( value>> ( 24 ) );
    data[offset+5] = ( uint8_t ) ( value>> ( 16 ) );
    data[offset+6] = ( uint8_t ) ( value>> ( 8 ) );
    data[offset+7] = ( uint8_t ) ( value>> ( 0 ) );
}



#ifdef __cplusplus
}
#endif

/*@}*/

#endif
