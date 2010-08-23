#ifndef US_REACTOR_H
#define US_REACTOR_H

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
#include "us_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** \defgroup poll_reactor reactor  */
    
    /** \ingroup poll_reactor */
    /*@{*/
    
    struct us_reactor_s;
    struct us_reactor_item_s;
    /*@}*/
    
    /** \ingroup poll_reactor */
    /** \defgroup reactor_item reactor_item  */
    /*@{*/
    typedef struct us_reactor_item_s
    {
        void ( *destroy ) ( struct us_reactor_item_s *self );
        bool ( *tick ) ( struct us_reactor_item_s *self );
        bool ( *readable ) ( struct us_reactor_item_s *self );
        bool ( *writable ) ( struct us_reactor_item_s *self );
        
        struct us_reactor_item_s *next;
        struct us_reactor_s *reactor;
        void *extra;
        int fd;
        bool wake_on_readable;
        bool wake_on_writable;
    } us_reactor_item_t;
    
    /**
    */
    typedef us_reactor_item_t * ( *us_reactor_item_create_proc_t ) ( void );
    
    /**
    */
    us_reactor_item_t * us_reactor_item_create ( void );
    
    /**
    */
    typedef bool ( *us_reactor_item_init_proc_t ) (
        us_reactor_item_t *self
    );
    
    /**
    */
    bool us_reactor_item_init ( us_reactor_item_t *self );
    
    /**
    */
    void us_reactor_item_destroy ( us_reactor_item_t *self );
    
    /*@}*/
    
    /** \ingroup poll_reactor */
    /** \defgroup reactor reactor  */
    /*@{*/
    typedef struct us_reactor_s
    {
        void ( *destroy ) ( struct us_reactor_s *self );
        
        us_reactor_item_t *items;
        int timeout;
        int max_items;
        int num_items;
        struct pollfd *poll_items;
        bool ( *poll ) ( struct us_reactor_s *self, int timeout );
        bool ( *add_item ) ( struct us_reactor_s *self, us_reactor_item_t *item );
        bool ( *remove_item ) ( struct us_reactor_s *self, us_reactor_item_t *item );
    } us_reactor_t;
    
    /**
    */
    bool us_reactor_init ( us_reactor_t *self, int max_items );
    
    /**
    */
    void us_reactor_destroy ( us_reactor_t *self );
    
    /**
    */
    bool us_reactor_poll ( us_reactor_t *self, int timeout );
    
    /**
    */
    bool us_reactor_add_item ( us_reactor_t *self, us_reactor_item_t *item );
    
    /**
    */
    bool us_reactor_remove_item ( us_reactor_t *self, us_reactor_item_t *item );
    
    /**
    */
    bool us_reactor_create_server (
        us_reactor_t *self,
        const char *server_host,
        const char *server_port,
        int ai_socktype, /* SOCK_DGRAM or SOCK_STREAM */
        us_reactor_item_create_proc_t server_item_create,
        us_reactor_item_init_proc_t server_item_init
    );
    /*@}*/
    
    
    /** \addtogroup reactor_item_tcp_server reactor_item_tcp_server
    */
    /*@{*/
    
    typedef struct us_reactor_item_tcp_server_s
    {
        us_reactor_item_t base;
        us_reactor_item_create_proc_t client_item_create;
        us_reactor_item_init_proc_t client_item_init;
    } us_reactor_item_tcp_server_t;
    
    /**
    */
    us_reactor_item_t *us_reactor_item_tcp_server_create ( void );
    
    /**
    */
    bool us_reactor_item_tcp_server_init (
        us_reactor_item_t *self,
        us_reactor_item_create_proc_t client_item_create,
        us_reactor_item_init_proc_t client_item_init
    );
    
    /**
    */
    bool us_reactor_item_tcp_server_readable (
        us_reactor_item_t *self
    );
    
    /*@}*/
    
    /** \ingroup poll_reactor */
    /** \defgroup reactor_item_tcp reactor_item_tcp  */
    /*@{*/
    typedef struct us_reactor_item_tcp_s
    {
        us_reactor_item_t base;
        int xfer_buf_size;
        char *xfer_buf;
        us_queue_t outgoing_queue;
        us_queue_t incoming_queue;
        
        bool ( *connected ) (
            struct us_reactor_item_tcp_s *self,
            struct sockaddr *addr,
            socklen_t addrlen
        );
        bool ( *tick ) (
            struct us_reactor_item_tcp_s *self
        );
        bool ( *readable ) (
            struct us_reactor_item_tcp_s *self
        );
    } us_reactor_item_tcp_t;
    
    
    us_reactor_item_t * us_reactor_item_tcp_create ( void );
    
    bool us_reactor_item_tcp_init (
        us_reactor_item_t *self,
        int queue_buf_size,
        int xfer_buf_size
    );
    
    void us_reactor_item_tcp_destroy (
        us_reactor_item_t *self
    );
    
    bool us_reactor_item_tcp_tick (
        us_reactor_item_t *self
    );
    
    bool us_reactor_item_tcp_readable (
        us_reactor_item_t *self
    );
    
    bool us_reactor_item_tcp_writable (
        us_reactor_item_t *self
    );
    
    /*@}*/
    
    
#ifdef __cplusplus
}
#endif

#endif
