#ifndef US_HTTP_SERVER_H
#define US_HTTP_SERVER_H


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

#include "us_buffer.h"
#include "us_net.h"
#include "us_reactor.h"
#include "us_logger.h"
#include "us_http.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define US_HTTP_SERVER_HANDLER_REQUEST_HEADER_SIZE (8192)
#define US_HTTP_SERVER_HANDLER_RESPONSE_HEADER_SIZE (8192)
#define US_HTTP_SERVER_HANDLER_LOCAL_BUFFER_SIZE (16384)

    typedef struct us_http_server_app_s
    {
        us_allocator_t *m_allocator;
        void (*destroy)(
            struct us_http_server_app_s *self
        );
        bool (*path_match)(
            struct us_http_server_app_s *self,
            const char *path
        );
        int (*dispatch)(
            struct us_http_server_app_s *self,
            const us_http_request_header_t *request_header,
            const us_buffer_t *request_content,
            us_http_response_header_t *response_header,
            us_buffer_t *response_content
        );
        struct us_http_server_app_s *m_next;
    } us_http_server_app_t;

    bool us_http_server_app_init( us_http_server_app_t *self, us_allocator_t *allocator );
    void us_http_server_app_destroy( us_http_server_app_t *self );
    bool us_http_server_app_path_match(us_http_server_app_t *self, const char *url );
    int us_http_server_app_dispatch(
        us_http_server_app_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );

    typedef struct us_http_server_director_s
    {
        us_allocator_t *m_allocator;
        void (*destroy)( struct us_http_server_director_s *self );
        int (*dispatch)(
            struct us_http_server_director_s *self,
            const us_http_request_header_t *request_header,
            const us_buffer_t *request_content,
            us_http_response_header_t *response_header,
            us_buffer_t *response_content
        );
        us_http_server_app_t *m_apps;
        us_http_server_app_t *m_last_app;

    } us_http_server_director_t;


    us_http_server_director_t *us_http_server_director_init( us_http_server_director_t *self, us_allocator_t *allocator );

    void us_http_server_director_destroy( us_http_server_director_t *self );

    bool us_http_server_director_add_app( us_http_server_director_t *self, us_http_server_app_t *m_app );

    int us_http_server_director_dispatch(
        us_http_server_director_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );

    typedef enum us_http_server_handler_state_e
    {
        us_http_server_handler_state_waiting_for_connection,
        us_http_server_handler_state_receiving_request_header,
        us_http_server_handler_state_receiving_request_content,
        us_http_server_handler_state_sending_response_header,
        us_http_server_handler_state_sending_response_content
    } us_http_server_handler_state_t;

    typedef struct us_http_server_handler_s
    {
        us_reactor_handler_tcp_t m_base;

        us_simple_allocator_t m_local_allocator;
        void *m_local_allocator_buffer;

        us_http_request_header_t *m_request_header;
        us_http_response_header_t *m_response_header;
        us_buffer_t *m_response_content;

        us_http_server_handler_state_t m_state;
        int32_t m_byte_count;
        int32_t m_todo_count;

        us_http_server_director_t *m_director;

    } us_http_server_handler_t;


    us_reactor_handler_t * us_http_server_handler_create(us_allocator_t *allocator);

    bool us_http_server_handler_init(
        us_reactor_handler_t *self,
        us_allocator_t *allocator,
        int fd,
        void *extra,
        int32_t max_request_buffer_size,
        int32_t max_response_buffer_size,
        us_http_server_director_t *director
    );

    void us_http_server_handler_destroy(
        us_reactor_handler_t *self
    );

    bool us_http_server_handler_connected(
        us_reactor_handler_tcp_t *self,
        struct sockaddr *addr,
        socklen_t addrlen
    );

    bool us_http_server_handler_readable_request_header(
        us_reactor_handler_tcp_t *self
    );

    bool us_http_server_handler_parse_request_header(
        us_http_server_handler_t *self
    );

    bool us_http_server_handler_readable_request_content(
        us_reactor_handler_tcp_t *self
    );

    bool us_http_server_handler_eof_request_content(
        us_reactor_handler_tcp_t *self
    );

    bool us_http_server_handler_eof_response_header(
        us_reactor_handler_tcp_t *self
    );

    bool us_http_server_handler_eof_response_content(
        us_reactor_handler_tcp_t *self
    );

    bool us_http_server_handler_dispatch(
        us_http_server_handler_t *self
    );

    static inline void us_http_server_handler_set_state_waiting_for_connection(
        us_http_server_handler_t *self
    )
    {
        us_queue_init(
            &self->m_base.m_incoming_queue,
            self->m_base.m_incoming_queue.m_buf,
            self->m_base.m_incoming_queue.m_buf_size
        );
        self->m_state = us_http_server_handler_state_waiting_for_connection;
        self->m_base.readable = 0;
        self->m_base.incoming_eof = 0;
        self->m_base.writable = 0;
        self->m_base.connected = us_http_server_handler_connected;
        self->m_byte_count = 0;
        self->m_todo_count = 0;
        self->m_base.m_base.m_wake_on_readable = true;
        self->m_base.m_base.m_wake_on_writable = false;
    }

    static inline void us_http_server_handler_set_state_receiving_request_header(
        us_http_server_handler_t *self
    )
    {
        self->m_state = us_http_server_handler_state_receiving_request_header;
        self->m_base.readable = us_http_server_handler_readable_request_header;
        self->m_base.incoming_eof = 0;
        self->m_base.writable = 0;
        self->m_base.connected = 0;
        self->m_byte_count = 0;
        self->m_base.m_base.m_wake_on_readable = true;
        self->m_base.m_base.m_wake_on_writable = false;
    }

    static inline void us_http_server_handler_set_state_receiving_request_content(
        us_http_server_handler_t *self
    )
    {
        self->m_state = us_http_server_handler_state_receiving_request_content;
        self->m_base.readable = us_http_server_handler_readable_request_content;
        self->m_base.incoming_eof = us_http_server_handler_eof_request_content;
        self->m_base.writable = 0;
        self->m_base.connected = 0;
        self->m_byte_count = 0;
        self->m_base.m_base.m_wake_on_readable = true;
        self->m_base.m_base.m_wake_on_writable = false;
    }

    static inline void us_http_server_handler_set_state_sending_response_header(
        us_http_server_handler_t *self
    )
    {
        self->m_state = us_http_server_handler_state_sending_response_header;
        self->m_base.readable = 0;
        self->m_base.incoming_eof = us_http_server_handler_eof_response_header;
        self->m_base.writable = 0;
        self->m_base.connected = 0;
        self->m_byte_count = 0;
        self->m_base.m_base.m_wake_on_readable = false;
        self->m_base.m_base.m_wake_on_writable = true;
    }

    static inline void us_http_server_handler_set_state_sending_response_content(
        us_http_server_handler_t *self
    )
    {
        self->m_state = us_http_server_handler_state_sending_response_content;
        self->m_base.readable = 0;
        self->m_base.incoming_eof = us_http_server_handler_eof_response_content;
        self->m_base.writable = 0;
        self->m_base.connected = 0;
        self->m_byte_count = 0;
        self->m_base.m_base.m_wake_on_readable = false;
        self->m_base.m_base.m_wake_on_writable = true;
    }


    void us_http_server_handler_closed(
        us_reactor_handler_tcp_t *self
    );


#ifdef __cplusplus
}
#endif



#endif

