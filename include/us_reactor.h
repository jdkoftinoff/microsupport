#ifndef US_REACTOR_H
#define US_REACTOR_H

/*
Copyright (c) 2010, Jeff Koftinoff <jeff.koftinoff@ieee.org>
All rights reserved.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "us_world.h"
#include "us_queue.h"
#include "us_allocator.h"

#if !defined(US_REACTOR_USE_POLL) && !defined(US_REACTOR_USE_SELECT)
# define US_REACTOR_USE_SELECT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /** \defgroup poll_reactor reactor  */

    /** \ingroup poll_reactor */
    /*@{*/

    struct us_reactor_s;
    struct us_reactor_handler_s;
    /*@}*/

    /** \ingroup poll_reactor */
    /** \defgroup reactor_item reactor_item  */
    /*@{*/
    typedef struct us_reactor_handler_s
    {
        void ( *destroy ) ( struct us_reactor_handler_s *self );
        bool ( *tick ) ( struct us_reactor_handler_s *self );
        bool ( *readable ) ( struct us_reactor_handler_s *self );
        bool ( *writable ) ( struct us_reactor_handler_s *self );
        us_allocator_t *m_allocator;
        struct us_reactor_handler_s *m_next;
        struct us_reactor_s *m_reactor;
        void *m_extra;
        int m_fd;
        bool m_wake_on_readable;
        bool m_wake_on_writable;
    } us_reactor_handler_t;

    /**
    */
    typedef us_reactor_handler_t * ( *us_reactor_handler_create_proc_t ) ( us_allocator_t *allocator  );

    /**
    */
    us_reactor_handler_t * us_reactor_handler_create ( us_allocator_t *allocator  );

    /**
    */
    typedef bool ( *us_reactor_handler_init_proc_t ) (
        us_reactor_handler_t *self,
        us_allocator_t *allocator,
        int fd,
        void *extra
    );

    /**
    */
    bool us_reactor_handler_init ( us_reactor_handler_t *self, us_allocator_t *allocator, int fd, void *extra );

    /**
    */
    void us_reactor_handler_destroy ( us_reactor_handler_t *self );

    /*@}*/

    /** \ingroup poll_reactor */
    /** \defgroup reactor reactor  */
    /*@{*/
    typedef struct us_reactor_s
    {
        void ( *destroy ) ( struct us_reactor_s *self );
        us_allocator_t *m_allocator;
        us_reactor_handler_t *m_handlers;
        int m_timeout;
        int m_max_handlers;
        int m_num_handlers;
#if defined(US_REACTOR_USE_POLL)
        struct pollfd *m_poll_handlers;
#elif defined(US_REACTOR_USE_SELECT)
        fd_set m_read_fds;
        fd_set m_write_fds;
#else
# error us_reactor needs implementation
#endif

        bool ( *poll ) ( struct us_reactor_s *self, int timeout );
        bool ( *add_item ) ( struct us_reactor_s *self, us_reactor_handler_t *item );
        bool ( *remove_item ) ( struct us_reactor_s *self, us_reactor_handler_t *item );
    } us_reactor_t;

    /**
    */
    bool us_reactor_init ( us_reactor_t *self, us_allocator_t *allocator, int max_handlers );

    /**
    */
    void us_reactor_destroy ( us_reactor_t *self );

    /**
    */
    bool us_reactor_poll ( us_reactor_t *self, int timeout );

    /**
    */
    bool us_reactor_add_item ( us_reactor_t *self, us_reactor_handler_t *item );

    /**
    */
    bool us_reactor_remove_item ( us_reactor_t *self, us_reactor_handler_t *item );

    /**
    */
    bool us_reactor_create_server (
        us_reactor_t *self,
        us_allocator_t *allocator,
        const char *server_host,
        const char *server_port,
        int ai_socktype, /* SOCK_DGRAM or SOCK_STREAM */
        void *client_extra,
        us_reactor_handler_create_proc_t server_handler_create,
        us_reactor_handler_init_proc_t server_handler_init
    );

    /**
    */
    int us_reactor_tcp_blocking_connect (
        const char *connect_host,
        const char *connect_port
    );


    bool us_reactor_create_tcp_client (
        us_reactor_t *self,
        us_allocator_t *allocator,
        const char *server_host,
        const char *server_port,
        void *extra,
        us_reactor_handler_create_proc_t client_handler_create,
        us_reactor_handler_init_proc_t client_handler_init
    );

    /*@}*/



    /** \addtogroup reactor_handler_tcp_server reactor_handler_tcp_server
    */
    /*@{*/

    typedef struct us_reactor_handler_tcp_server_s
    {
        us_reactor_handler_t m_base;
        us_reactor_handler_create_proc_t client_handler_create;
        us_reactor_handler_init_proc_t client_handler_init;
    } us_reactor_handler_tcp_server_t;

    /**
    */
    us_reactor_handler_t *us_reactor_handler_tcp_server_create ( us_allocator_t *allocator  );

    /**
    */
    bool us_reactor_handler_tcp_server_init (
        us_reactor_handler_t *self,
        us_allocator_t *allocator,                                              
        int fd,
        void *client_extra,
        us_reactor_handler_create_proc_t client_handler_create,
        us_reactor_handler_init_proc_t client_handler_init
    );

    /**
    */
    bool us_reactor_handler_tcp_server_readable (
        us_reactor_handler_t *self
    );

    /*@}*/

    /** \ingroup poll_reactor */
    /** \defgroup reactor_handler_tcp reactor_handler_tcp  */
    /*@{*/
    typedef struct us_reactor_handler_tcp_s
    {
        us_reactor_handler_t m_base;
        int m_xfer_buf_size;
        char *m_xfer_buf;
        us_queue_t m_outgoing_queue;
        us_queue_t m_incoming_queue;

        void ( *close ) (
            struct us_reactor_handler_tcp_s *self
        );

        bool ( *connected ) (
            struct us_reactor_handler_tcp_s *self,
            struct sockaddr *addr,
            socklen_t addrlen
        );
        bool ( *tick ) (
            struct us_reactor_handler_tcp_s *self
        );
        bool ( *readable ) (
            struct us_reactor_handler_tcp_s *self
        );
        void ( *closed ) (
            struct us_reactor_handler_tcp_s *self
        );
    } us_reactor_handler_tcp_t;


    us_reactor_handler_t * us_reactor_handler_tcp_create ( us_allocator_t *allocator );

    bool us_reactor_handler_tcp_init (
        us_reactor_handler_t *self,
        us_allocator_t *allocator,
        int fd,
        void *extra,
        int queue_buf_size,
        int xfer_buf_size
    );

    void us_reactor_handler_tcp_destroy (
        us_reactor_handler_t *self
    );

    bool us_reactor_handler_tcp_tick (
        us_reactor_handler_t *self
    );

    bool us_reactor_handler_tcp_readable (
        us_reactor_handler_t *self
    );

    bool us_reactor_handler_tcp_writable (
        us_reactor_handler_t *self
    );

    void us_reactor_handler_tcp_close (
        us_reactor_handler_tcp_t *self
    );


    /*@}*/


#ifdef __cplusplus
}
#endif

#endif
