#ifndef US_OSC_SENDER_H
#define US_OSC_SENDER_H


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
#include "us_osc_msg.h"
#include "us_reactor.h"
#ifdef __cplusplus
extern "C" {
#endif


    typedef struct us_osc_sender_s
    {
        void (*destroy)(
            struct us_osc_sender_s *self
        );
        bool (*can_send)(
            struct us_osc_sender_s *self
        );
        bool (*send_msg)(
            struct us_osc_sender_s *self,
            const us_osc_msg_t *msg
        );
        bool (*form_and_send_msg)(
            struct us_osc_sender_s *self,
            const char *address,
            const char *typetags,
            ...
        );
    } us_osc_sender_t;

    us_osc_sender_t *us_osc_sender_create( void );
    bool us_osc_sender_init( us_osc_sender_t *self, void *US_UNUSED(extra) );
    void us_osc_sender_destroy( us_osc_sender_t *US_UNUSED(self) );
    bool us_osc_sender_send_msg( us_osc_sender_t *US_UNUSED(self), const us_osc_msg_t *US_UNUSED(msg) );
    bool us_osc_sender_can_send( us_osc_sender_t *US_UNUSED(self) );
    bool us_osc_sender_form_and_send_msg( us_osc_sender_t *self, const char *address, const char *typetags, ... );


    typedef struct us_osc_multisender_item_s
    {
        us_osc_sender_t *m_sender;
        struct us_osc_multisender_item_s *m_next;
        struct us_osc_multisender_item_s *m_prev;
    } us_osc_multisender_item_t;

#define US_OSC_MULTISENDER_MAX_ITEMS (16)

    typedef struct us_osc_multisender_s
    {
        us_osc_sender_t m_base;
        us_allocator_t *m_allocator;
        us_osc_multisender_item_t *m_first_item;
        us_osc_multisender_item_t *m_last_item;
        us_osc_multisender_item_t *m_first_empty_item;
        us_osc_multisender_item_t m_items[US_OSC_MULTISENDER_MAX_ITEMS];
    } us_osc_multisender_t;

    us_osc_multisender_t *us_osc_multisender_create( us_allocator_t *allocator );
    bool us_osc_multisender_init( us_osc_multisender_t *self );
    void us_osc_multisender_destroy( us_osc_sender_t *self );
    bool us_osc_multisender_send_msg( us_osc_sender_t *self, const us_osc_msg_t *msg );
    bool us_osc_multisender_can_send( us_osc_sender_t *self );
    bool us_osc_multisender_add_sender( us_osc_multisender_t *self, us_osc_sender_t *item );
    bool us_osc_multisender_remove_sender( us_osc_multisender_t *self, us_osc_sender_t *item );

#ifdef __cplusplus
}
#endif

#endif
