#ifndef US_REACTOR_HANDLER_TCP_H
#define US_REACTOR_HANDLER_TCP_H

/*
Copyright (c) 2013, J.D. Koftinoff Software, Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"
#include "us_reactor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*/
int us_reactor_connect_or_open( const char *fname );

/**
*/
int us_reactor_tcp_blocking_connect( const char *connect_host, const char *connect_port );

typedef bool ( *us_reactor_handler_tcp_client_init_proc_t )( us_reactor_handler_t *self,
                                                             us_allocator_t *allocator,
                                                             int fd,
                                                             void *extra,
                                                             ssize_t queue_buf_size,
                                                             const char *client_host,
                                                             const char *client_port,
                                                             bool keep_open );

bool us_reactor_create_tcp_client( us_reactor_t *self,
                                   us_allocator_t *allocator,
                                   void *extra,
                                   ssize_t queue_buf_size,
                                   const char *server_host,
                                   const char *server_port,
                                   bool keep_open,
                                   us_reactor_handler_create_proc_t client_handler_create,
                                   us_reactor_handler_tcp_client_init_proc_t client_handler_init );

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
us_reactor_handler_t *us_reactor_handler_tcp_server_create( us_allocator_t *allocator );

/**
*/
bool us_reactor_handler_tcp_server_init( us_reactor_handler_t *self,
                                         us_allocator_t *allocator,
                                         int fd,
                                         void *client_extra,
                                         us_reactor_handler_create_proc_t client_handler_create,
                                         us_reactor_handler_init_proc_t client_handler_init );

/**
*/
bool us_reactor_handler_tcp_server_readable( us_reactor_handler_t *self );

/*@}*/

/** \ingroup poll_reactor */
/** \defgroup reactor_handler_tcp reactor_handler_tcp  */
/*@{*/
typedef struct us_reactor_handler_tcp_s
{
    us_reactor_handler_t m_base;
    us_buffer_t m_outgoing_queue;
    us_buffer_t m_incoming_queue;

    void ( *close )( struct us_reactor_handler_tcp_s *self );

    bool ( *connected )( struct us_reactor_handler_tcp_s *self, struct sockaddr *addr, socklen_t addrlen );
    bool ( *tick )( struct us_reactor_handler_tcp_s *self );
    bool ( *readable )( struct us_reactor_handler_tcp_s *self );
    bool ( *writable )( struct us_reactor_handler_tcp_s *self );
    bool ( *incoming_eof )( struct us_reactor_handler_tcp_s *self );
    void ( *closed )( struct us_reactor_handler_tcp_s *self );
} us_reactor_handler_tcp_t;

us_reactor_handler_t *us_reactor_handler_tcp_create( us_allocator_t *allocator );

bool us_reactor_handler_tcp_init(
    us_reactor_handler_t *self, us_allocator_t *allocator, int fd, void *extra, int queue_buf_size );

void us_reactor_handler_tcp_destroy( us_reactor_handler_t *self );

bool us_reactor_handler_tcp_tick( us_reactor_handler_t *self );

bool us_reactor_handler_tcp_readable( us_reactor_handler_t *self );

bool us_reactor_handler_tcp_writable( us_reactor_handler_t *self );

void us_reactor_handler_tcp_close( us_reactor_handler_tcp_t *self );

/*@}*/

/** \ingroup poll_reactor */
/** \defgroup reactor_handler_tcp_client reactor_handler_tcp_client  */
/*@{*/
typedef struct us_reactor_handler_tcp_client_s
{
    us_reactor_handler_tcp_t m_base;
    const char *m_client_host;
    const char *m_client_port;
    bool m_keep_open;
    bool m_is_connected;
    bool m_try_once;
} us_reactor_handler_tcp_client_t;

us_reactor_handler_t *us_reactor_handler_tcp_client_create( us_allocator_t *allocator );

bool us_reactor_handler_tcp_client_init( us_reactor_handler_t *self,
                                         us_allocator_t *allocator,
                                         int fd,
                                         void *extra,
                                         int queue_buf_size,
                                         const char *client_host,
                                         const char *client_port,
                                         bool keep_open );

void us_reactor_handler_tcp_client_destroy( us_reactor_handler_t *self );

bool us_reactor_handler_tcp_client_tick( us_reactor_handler_tcp_t *self );

void us_reactor_handler_tcp_client_closed( struct us_reactor_handler_tcp_s *self );

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
