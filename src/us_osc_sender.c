#include "us_world.h"
#include "us_osc_sender.h"

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


us_osc_sender_t *us_osc_sender_create( void )
{
    return (us_osc_sender_t *)calloc( sizeof( us_osc_sender_t), 1 );
}

bool us_osc_sender_init( us_osc_sender_t *self, void *US_UNUSED(extra) )
{
    self->destroy = us_osc_sender_destroy;
    self->can_send = us_osc_sender_can_send;
    self->send_msg = us_osc_sender_send_msg;
    self->form_and_send_msg = us_osc_sender_form_and_send_msg;
    return true;
}

void us_osc_sender_destroy( us_osc_sender_t *US_UNUSED(self) )
{
    /* nothing to do */
}

bool us_osc_sender_send_msg( us_osc_sender_t *US_UNUSED(self), const us_osc_msg_t *US_UNUSED(msg) )
{
    /* base class does nothing to send */
    return false;
}

bool us_osc_sender_can_send( us_osc_sender_t *US_UNUSED(self) )
{
    /* base class can not send messages every */
    return false;
}


bool us_osc_sender_form_and_send_msg( us_osc_sender_t *self, const char *address, const char *typetags, ... )
{
    bool r=false;
    us_osc_msg_t *msg;
    us_simple_allocator_t tmp_allocator;
    char buf[2048];
    va_list ap;
    us_simple_allocator_init( &tmp_allocator, buf, sizeof(buf) );
    va_start( ap, typetags );
    msg = us_osc_msg_vform( &tmp_allocator.m_base, address, typetags, ap );
    if ( msg )
    {
        r=self->send_msg( self, msg );
        msg->destroy( msg );
    }
    else
    {
        us_log_error( "unable to form osc message %s %s",address,typetags);
    }
    va_end( ap );
    return r;
}



us_osc_multisender_t *us_osc_multisender_create( us_allocator_t *allocator )
{
    us_osc_multisender_t *self = us_new( allocator, us_osc_multisender_t );
    return self;
}

bool us_osc_multisender_init( us_osc_multisender_t *self )
{
    int i;
    self->m_base.destroy = us_osc_multisender_destroy;
    self->m_base.can_send = us_osc_multisender_can_send;
    self->m_base.send_msg = us_osc_multisender_send_msg;
    self->m_base.form_and_send_msg = us_osc_sender_form_and_send_msg;
    self->m_first_item=0;
    self->m_last_item=0;
    self->m_first_empty_item=&self->m_items[0];
    for( i=0; i<US_OSC_MULTISENDER_MAX_ITEMS; ++i )
    {
        self->m_items[i].m_sender = 0;
        if( i>0 )
        {
            self->m_items[i].m_prev = &self->m_items[i-1];
        }
        else
        {
            self->m_items[i].m_prev = 0;
        }
        if( i!=US_OSC_MULTISENDER_MAX_ITEMS-1 )
        {
            self->m_items[i].m_next = &self->m_items[i+1];
        }
        else
        {
            self->m_items[i].m_next = 0;
        }
    }
    return true;
}

void us_osc_multisender_destroy( us_osc_sender_t *self )
{
}

bool us_osc_multisender_send_msg( us_osc_sender_t *self_, const us_osc_msg_t *msg )
{
    bool r=true;
    us_osc_multisender_t *self = (us_osc_multisender_t *)self_;
    us_osc_multisender_item_t *item = self->m_first_item;
    while( item )
    {
        if( item->m_sender )
        {
            r&=item->m_sender->send_msg( item->m_sender, msg );
        }
        item = item->m_next;
    }
    return r;
}

bool us_osc_multisender_can_send( us_osc_sender_t *self_ )
{
    bool r=true;
    us_osc_multisender_t *self = (us_osc_multisender_t *)self_;
    us_osc_multisender_item_t *item = self->m_first_item;
    while( item )
    {
        if( item->m_sender )
        {
            r&=item->m_sender->can_send( item->m_sender );
        }
        item = item->m_next;
    }
    return r;
}

bool us_osc_multisender_add_sender( us_osc_multisender_t *self, us_osc_sender_t *item )
{
    bool r=false;
    us_osc_multisender_item_t *allocated_item = 0;
    if( self->m_first_empty_item )
    {
        allocated_item = self->m_first_empty_item;
        self->m_first_empty_item = allocated_item->m_next;
        allocated_item->m_sender = item;
        allocated_item->m_next = 0;
        if( self->m_first_item==0 )
        {
            self->m_first_item=allocated_item;
            self->m_last_item=allocated_item;
        }
        else
        {
            allocated_item->m_prev = self->m_last_item;
            self->m_last_item = allocated_item;
        }
        r=true;
    }
    return r;
}

bool us_osc_multisender_remove_sender( us_osc_multisender_t *self, us_osc_sender_t *sender )
{
    bool r=false;
    us_osc_multisender_item_t *item = self->m_first_item;
    while( item )
    {
        if( item->m_sender == sender )
        {
            break;
        }
    }
    if( item )
    {
        item->m_sender = 0;
        item->m_prev = 0;
        if( self->m_first_empty_item )
        {
            self->m_first_empty_item->m_prev = item;
            item->m_next = self->m_first_empty_item;
        }
        else
        {
            item->m_next = 0;
            self->m_first_empty_item = item;
        }
        r=true;
    }
    return r;
}

