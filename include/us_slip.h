#ifndef US_SLIP_H
#define US_SLIP_H

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
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"

#include "us_buffer.h"

/**
 \addtogroup us_slip  Slip Encoding/Decoding ( RFC-1055 http://www.faqs.org/rfcs/rfc1055.html )
 */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

  #define US_SLIP_END (0300) /**! indicates end of packet */
  #define US_SLIP_ESC (0333) /**! indicates byte stuffing */
  #define US_SLIP_ESC_END (0334) /**! ESC ESC_END means END data byte */
  #define US_SLIP_ESC_ESC (0335) /**! ESC ESC_ESC means ESC data byte */

  /**
      State machine states for parsing slip encoded frames
  */
  typedef enum us_slip_state_enum {
    us_slip_state_before_packet,          /**! ignore any data between frames     */
    us_slip_state_in_packet,              /**! stuff data into buffer             */
    us_slip_state_in_esc                  /**! interpret next data code specially */
  } us_slip_state_t;

  struct us_slip_decoder_s;

  /** type for a callback procedure which is called whenever the slip decoder forms a complete packet */
  typedef void (*us_slip_decoder_proc)( struct us_slip_decoder_s *self, us_buffer_t *buf );

  /** slip framing decoder
   */
  typedef struct us_slip_decoder_s
  {
    us_slip_state_t m_state;
    us_buffer_t *m_buffer;
    us_slip_decoder_proc m_packet_formed_callback;
  } us_slip_decoder_t;

  /**
   Create a slip decoder object and associated buffer

   @param allocator allocator to allocate memory from for the slip decoder and the buffer
   @param max_packet_len maximum incoming packet length
   @param packet_formed_callback callback function that is called when a packet is formed
   @returns pointer to slip decoder initialized, or 0 on error
   */
  us_slip_decoder_t *
  us_slip_decoder_create(
                           us_allocator_t *allocator,
                           int32_t max_packet_len,
                           us_slip_decoder_proc packet_formed_callback
                           );

  /**
   Initialize a slip decoder object

   @param self pointer to allocated slip decoder object
   @param buffer pointer to allocated buffer object
   @param packet_formed_callback callback function that is called when a packet is formed
   @returns pointer to slip decoder initialized, or 0 on error
   */
  us_slip_decoder_t *
  us_slip_decoder_init(
                         us_slip_decoder_t *self,
                         us_buffer_t *buffer,
                         us_slip_decoder_proc packet_formed_callback
                         );

  /**
   Reset the state machine for a slip decoder object

   @param self pointer to allocated slip decoder object
   */
  void us_slip_decoder_reset(
                               us_slip_decoder_t *self
                               );

  /**
   Parse a block of raw data for slip framing and call the packet_formed_callback
   whenenever a full frame is formed in the buffer.

   @param self pointer to allocated slip decoder object
   @param data pointer to raw incoming data to parse
   @param data_len length of raw incoming data to parse
   @returns int count of packets parsed
   */
  int us_slip_decoder_parse(
                               us_slip_decoder_t *self,
                               const uint8_t *data,
                               int data_len
                               );

  /**
   Parse the entire contents of a buffer

   @param self pointer to allocated slip decoder object
   @param buffer pointer to buffer to parse
   @returns int count of packets parsed
   */
#  define us_slip_decoder_parse_buffer( self, buffer ) us_slip_decoder_parse( (self), (buffer)->m_buffer, (buffer)->m_cur_length )

  /** Encode a complete packet from src_buffer into
   dest_buffer with slip packet framing. Does not clear the
   dest_buffer first.

   @param dest_buffer destination buffer
   @param src_buffer source buffer
   @returns bool true on success, even if src_buffer was empty.
   */
  bool us_slip_encode(
                        us_buffer_t *dest_buffer,
                        us_buffer_t *src_buffer
                        );


#ifdef __cplusplus
}
#endif

/*@}*/

#endif
