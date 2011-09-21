#ifndef US_OSC_MSG_H
#define US_OSC_MSG_H

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

#include "us_world.h"

#include "us_allocator.h"
#include "us_buffer.h"
#include "us_print.h"
#ifdef __cplusplus
extern "C" {
#endif
    
#define US_OSC_MSG_MAX_ADDRESS_LENGTH (256)
#define US_OSC_MSG_MAX_TYPETAGS (128)

    /**
     \addtogroup us_osc_msg OSC Message and Bundle Objects
     */
    /*@{*/

    struct us_osc_msg_element_s;
    typedef struct us_osc_msg_element_s us_osc_msg_element_t;


    /**
     */
    typedef struct us_osc_msg_s
    {
        void (*destroy)( struct us_osc_msg_s *self );

        us_osc_msg_element_t *
        (*append)(
            struct us_osc_msg_s *self,
            us_osc_msg_element_t *element
        );

        bool
        (*print)(
            const struct us_osc_msg_s *self,
            us_print_t *printer
        );

        bool
        (*flatten)(
            const struct us_osc_msg_s *self,
            us_buffer_t *buf,
            int32_t *length
        );

        us_allocator_t *m_allocator;
        char *m_address;
        uint32_t m_address_code;
        us_osc_msg_element_t *m_first_element;
        us_osc_msg_element_t *m_last_element;
        struct us_osc_msg_s *m_next;
    } us_osc_msg_t;


    us_osc_msg_t *
    us_osc_msg_create(
        us_allocator_t *allocator,
        const char *address
    );

    us_osc_msg_t *
    us_osc_msg_create_code(
        us_allocator_t *allocator,
        uint32_t address_code
    );

    void us_osc_msg_destroy( us_osc_msg_t *self );

    us_osc_msg_t *
    us_osc_msg_form(
        us_allocator_t *allocator,
        const char *address,
        const char *typetags,
        ...
    );

    us_osc_msg_t *
    us_osc_msg_vform(
        us_allocator_t *allocator,
        const char *address,
        const char *typetags,
        va_list args
    );


    us_osc_msg_element_t *
    us_osc_msg_append(
        us_osc_msg_t *self,
        us_osc_msg_element_t *element
    );

    bool
    us_osc_msg_flatten(
        const us_osc_msg_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    typedef const char * (*us_osc_msg_addrcode_typetag_map_proc)( uint32_t addrcode );

    us_osc_msg_t *
    us_osc_msg_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf,
        us_osc_msg_addrcode_typetag_map_proc addrcode_mapper
    );


    /*@}*/


    /**
     \addtogroup us_osc_msg_bundle Message Bundle
     */
    /*{*/

    typedef struct us_osc_msg_bundle_s
    {
        void (*destroy)(
            struct us_osc_msg_bundle_s *self
        );

        us_osc_msg_t *
        (*append)(
            struct us_osc_msg_bundle_s *self,
            us_osc_msg_t *msg
        );

        bool
        (*print)(
            const struct us_osc_msg_bundle_s *self,
            us_print_t *printer
        );

        bool
        (*flatten)(
            const struct us_osc_msg_bundle_s *self,
            us_buffer_t *buf,
            int32_t *length
        );

        uint32_t m_timetag_high;
        uint32_t m_timetag_low;

        us_osc_msg_t *m_first_msg;
        us_osc_msg_t *m_last_msg;
    } us_osc_msg_bundle_t;

    us_osc_msg_bundle_t *
    us_osc_msg_bundle_create(
        us_allocator_t *allocator,
        uint32_t timetag_high,
        uint32_t timetag_low
    );

    void
    us_osc_msg_bundle_destroy(
        us_osc_msg_bundle_t *self
    );

    us_osc_msg_t *
    us_osc_msg_bundle_append(
        us_osc_msg_bundle_t *self,
        us_osc_msg_t *msg
    );

    bool
    us_osc_msg_bundle_flatten(
        const us_osc_msg_bundle_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_bundle_t *
    us_osc_msg_bundle_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf,
        int32_t bundle_size,
        us_osc_msg_addrcode_typetag_map_proc addrmapper
    );

    /*@}*/


    /**
     \addtogroup us_osc_msg_element Message element base class
     */
    /*@{*/
    struct us_osc_msg_element_s
    {
        void (*destroy)( struct us_osc_msg_element_s *self, us_allocator_t *allocator );

        bool
        (*print)(
            const struct us_osc_msg_element_s *self,
            us_print_t *printer
        );

        bool
        (*flatten)(
            const struct us_osc_msg_element_s *self,
            us_buffer_t *buf,
            int32_t *length
        );

        struct us_osc_msg_element_s *m_next;
        char m_code;
    };

    /**
     * Initialize a message element structure with a specific ascii code.
     *
     * @param self ptr to element to init
     * @param code ascii code to initialize with
     * @return ptr to element that was initialized, or 0 upon error
     */
    us_osc_msg_element_t *
    us_osc_msg_element_init(
        us_osc_msg_element_t *self,
        int code
    );

    /**
     * Destroy a message element
     */

    void
    us_osc_msg_element_destroy(
        us_osc_msg_element_t *self,
        us_allocator_t *allocator
    );


    /**
     * Flatten a message element structure into a buffer
     *
     * @param self ptr to element to flatten
     * @param buf ptr to buffer to flatten to
     * @param length ptr to int32_t to store length of flattened image
     * @return bool true if no errors occurred
     */
    bool
    us_osc_msg_element_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    /**
     * Unflatten a message element structure from a buffer,
     * allocating the appropriate element structure via an allocator
     *
     * @param allocator ptr to allocator to allocate element from
     * @param buf ptr to buffer to unflatten
     * @param typetag the typetag of this element
     * @return ptr to element or 0 upon any error
     */

    us_osc_msg_element_t *
    us_osc_msg_element_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf,
        char typetag
    );


    /*@}*/

    /**
     \addtogroup us_osc_msg_elements All message elements
     */
    /*@{*/

    struct us_osc_msg_element_s;

    /**
     \addtogroup us_osc_msg_element_a 'a': 64 bit AVB style time tag element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_a_s
    {
        us_osc_msg_element_t m_base;
        uint32_t m_time_high;
        uint32_t m_time_low;
    } us_osc_msg_element_a_t;

    us_osc_msg_element_t *
    us_osc_msg_element_a_create(
        us_allocator_t *allocator,
        uint32_t time_high,
        uint32_t time_low
    );


    bool
    us_osc_msg_element_a_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_a_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/

    /**
     \addtogroup us_osc_msg_element_B 'B': osc message bundle element
     */
    /*@{*/

    typedef struct us_osc_msg_element_B_s
    {
        us_osc_msg_element_t m_base;
        us_osc_msg_bundle_t *m_bundle;
    } us_osc_msg_element_B_t;

    us_osc_msg_element_t *
    us_osc_msg_element_B_create(
        us_allocator_t *allocator,
        us_osc_msg_bundle_t *m_msg
    );

    /**
     * Destroy a message element B
     */

    void
    us_osc_msg_element_B_destroy(
        us_osc_msg_element_t *self,
        us_allocator_t *allocator
    );


    bool
    us_osc_msg_element_B_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_B_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/


    /**
     \addtogroup us_osc_msg_element_b 'b': blob element
     */
    /*@{*/

    typedef struct us_osc_msg_element_b_s
    {
        us_osc_msg_element_t m_base;
        uint8_t *m_data;
        int32_t m_length;
    } us_osc_msg_element_b_t;

    us_osc_msg_element_t *
    us_osc_msg_element_b_create(
        us_allocator_t *allocator,
        const uint8_t *data,
        int32_t length
    );

    /**
     * Destroy a message element b
     */

    void
    us_osc_msg_element_b_destroy(
        us_osc_msg_element_t *self,
        us_allocator_t *allocator
    );



    bool
    us_osc_msg_element_b_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_b_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/


    /**
     \addtogroup us_osc_msg_element_d 'd': 64 bit float element
     */
    /*@{*/
#if US_ENABLE_DOUBLE
    typedef struct us_osc_msg_element_d_s
    {
        us_osc_msg_element_t m_base;
        double m_value;
    } us_osc_msg_element_d_t;

    us_osc_msg_element_t *
    us_osc_msg_element_d_create(
        us_allocator_t *allocator,
        double value
    );

    bool
    us_osc_msg_element_d_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_d_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

#endif
    /*@}*/


    /**
     \addtogroup us_osc_msg_element_F 'F': False element
     */
    /*@{*/

    typedef struct us_osc_msg_element_F_s
    {
        us_osc_msg_element_t m_base;
    } us_osc_msg_element_F_t;

    us_osc_msg_element_t *
    us_osc_msg_element_F_create(
        us_allocator_t *allocator
    );

    bool
    us_osc_msg_element_F_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_F_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

    /*@}*/



    /**
     \addtogroup us_osc_msg_element_f 'f': 32 bit float element
     */
    /*@{*/
#if US_ENABLE_FLOAT
    typedef struct us_osc_msg_element_f_s
    {
        us_osc_msg_element_t m_base;
        float m_value;
    } us_osc_msg_element_f_t;

    us_osc_msg_element_t *
    us_osc_msg_element_f_create(
        us_allocator_t *allocator,
        float value
    );

    bool
    us_osc_msg_element_f_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_f_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

#endif
    /*@}*/

    /**
     \addtogroup us_osc_msg_element_h 'h': 64 bit integer element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_h_s
    {
        us_osc_msg_element_t m_base;
        uint32_t m_value_high;
        uint32_t m_value_low;
    } us_osc_msg_element_h_t;

    us_osc_msg_element_t *
    us_osc_msg_element_h_create(
        us_allocator_t *allocator,
        uint32_t value_high,
        uint32_t value_low
    );

    bool
    us_osc_msg_element_h_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_h_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/

    /**
     \addtogroup us_osc_msg_element_I 'I': Impulse element
     */
    /*@{*/

    typedef struct us_osc_msg_element_I_s
    {
        us_osc_msg_element_t m_base;
    } us_osc_msg_element_I_t;

    us_osc_msg_element_t *
    us_osc_msg_element_I_create(
        us_allocator_t *allocator
    );

    bool
    us_osc_msg_element_I_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_I_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

    /*@}*/

    /**
     \addtogroup us_osc_msg_element_i 'i': 32 bit integer element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_i_s
    {
        us_osc_msg_element_t m_base;
        int32_t m_value;
    } us_osc_msg_element_i_t;

    us_osc_msg_element_t *
    us_osc_msg_element_i_create(
        us_allocator_t *allocator,
        int32_t value
    );


    bool
    us_osc_msg_element_i_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_i_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/

    /**
     \addtogroup us_osc_msg_element_M 'M': osc message element
     */
    /*@{*/

    typedef struct us_osc_msg_element_M_s
    {
        us_osc_msg_element_t m_base;
        us_osc_msg_t *m_msg;
    } us_osc_msg_element_M_t;

    us_osc_msg_element_t *
    us_osc_msg_element_M_create(
        us_allocator_t *allocator,
        us_osc_msg_t *m_msg
    );

    /**
     * Destroy a message element M
     */

    void
    us_osc_msg_element_M_destroy(
        us_osc_msg_element_t *self,
        us_allocator_t *allocator
    );



    bool
    us_osc_msg_element_M_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_M_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/


    /**
     \addtogroup us_osc_msg_element_N 'N': Nil Element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_N_s
    {
        us_osc_msg_element_t m_base;
    } us_osc_msg_element_N_t;

    us_osc_msg_element_t *
    us_osc_msg_element_N_create(
        us_allocator_t *allocator
    );

    bool
    us_osc_msg_element_N_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_N_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

    /*@}*/



    /**
     \addtogroup us_osc_msg_element_s 's': string message element
     */
    /*@{*/
    typedef struct us_osc_msg_element_s_s
    {
        us_osc_msg_element_t m_base;
        char *m_value;
        int32_t m_length;
    } us_osc_msg_element_s_t;

    /**
     */
    us_osc_msg_element_t *
    us_osc_msg_element_s_create(
        us_allocator_t *allocator,
        const char *value
    );

    /**
     * Destroy a message element s
     */

    void
    us_osc_msg_element_s_destroy(
        us_osc_msg_element_t *self,
        us_allocator_t *allocator
    );


    bool
    us_osc_msg_element_s_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_s_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/

    /**
     \addtogroup us_osc_msg_element_T 'T': True element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_T_s
    {
        us_osc_msg_element_t m_base;
    } us_osc_msg_element_T_t;

    us_osc_msg_element_t *
    us_osc_msg_element_T_create(
        us_allocator_t *allocator
    );

    bool
    us_osc_msg_element_T_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_T_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );

    /*@}*/


    /**
     \addtogroup us_osc_msg_element_t 't': 64 bit NTP style time tag element
     */
    /*@{*/

    /**
     */
    typedef struct us_osc_msg_element_t_s
    {
        us_osc_msg_element_t m_base;
        uint32_t m_time_high;
        uint32_t m_time_low;
    } us_osc_msg_element_t_t;

    us_osc_msg_element_t *
    us_osc_msg_element_t_create(
        us_allocator_t *allocator,
        uint32_t time_high,
        uint32_t time_low
    );

    bool
    us_osc_msg_element_t_flatten(
        const us_osc_msg_element_t *self,
        us_buffer_t *buf,
        int32_t *length
    );

    us_osc_msg_element_t *
    us_osc_msg_element_t_unflatten(
        us_allocator_t *allocator,
        us_buffer_t *buf
    );


    /*@}*/



    /**
     \addtogroup us_osc_msg_inspector Inspector functions to differentiate between OSC Messages and OSC Bundles
     */

    /*@{*/
    /**
     Check if buffer contains an OSC Bundle

     @param buffer buffer to check
     @return bool true if it is a valid OSC Bundle (multiple messages)
     */
    bool
    us_osc_msg_is_msg_bundle(
        const us_buffer_t *buffer
    );

    /**
     Check if buffer contains an OSC Message

     @param buffer buffer to check
     @return bool true if it is a valid OSC Message
     */
    bool
    us_osc_msg_is_msg(
        const us_buffer_t *buffer
    );

    /**
     Check if buffer contains an OSC message with quadlet address-code

     @param buffer buffer to check
     @return bool true if it is a valid OSC Message with address-code
     */

    bool
    us_osc_msg_is_msg_code(
        const us_buffer_t *buffer
    );

    /**
     Check if address code has the flag that says the message has a typetag string

     @param uint32_t address code
     @return bool true if it is an address code with a typetag
     */
    static inline bool
    us_osc_msg_address_has_typetags(
        uint32_t address_code
    )
    {
        return (address_code & 0xc0000000)==0xc0000000;
    }

    /**
     Parse either a bundle or an OSC message

     @param allocator allocator to use to allocate msg or bundle
     @param msg pointer to msg pointer that will be filled in if buffer contains message
     @param bundle pointer to bundler pointer that will be filled in if buffer contains bundle
     @param buffer pointer to buffer to parse
     @return bool true if msg or bundle was parsed. One of msg or bundle will be non-zero
     */
    bool
    us_osc_parse(
        us_allocator_t *allocator,
        us_osc_msg_t **msg,
        us_osc_msg_bundle_t **bundle,
        us_buffer_t *buffer,
        int32_t packet_size,
        us_osc_msg_addrcode_typetag_map_proc addrmapper
    );

    /*@}*/



    /*@}*/
#ifdef __cplusplus
}
#endif

#endif
