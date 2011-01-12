#ifndef US_HTTPD_REACTOR_H
#define US_HTTPD_REACTOR__H


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

#include "us_http.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct us_httpd_request_handler_s
    {
        us_allocator_t *m_allocator;
        void (*destroy)(
                struct us_httpd_request_handler_s *self,
                );
        bool (*url_match)(
                struct us_httpd_request_handler_s *self,
                us_http_request_header_t *request_header
                );
        int (*dispatch)(
                struct us_httpd_request_handler_s *self,
                us_http_request_header_t *request_header
                );
        bool (*readable)(
                struct us_httpd_request_handler_s *self,
                us_queue_t *incoming_queue
                );
        bool (*writable)(
                struct us_httpd_request_handler_s *self,
                us_queue_t *outgoing_queue
                );
    } us_httpd_request_handler_t;

    typedef struct us_httpd_director_s
    {
        void (*destroy)( struct us_httpd_director_s *self );

    } us_httpd_director_t;

    typedef struct us_httpd_reactor_handler_s
    {
        us_reactor_handler_t m_base;
        us_httpd_director_t *m_director;
    } us_httpd_reactor_handler_t;


    us_reactor_handler_t *
    us_httpd_reactor_handler_create(
            us_allocator_t *allocator
            );

    bool us_httpd_reactor_handler_init(
            us_reactor_handler_t *self_,
            us_allocator_t *allocator,
            int fd,
            void *extra,

            );

    bool us_http_server_reactor_handler_connected (
        us_reactor_handler_tcp_t *self,
        struct sockaddr *addr,
        socklen_t addrlen
    );

    bool us_http_server_reactor_handler_tick (
        us_reactor_handler_tcp_t *self_
    );

    bool us_http_server_reactor_handler_readable(
        us_reactor_handler_tcp *self_
    );

    bool us_http_server_reactor_handler_writable(
        us_reactor_handler_tcp *self_
    );

#ifdef __cplusplus
}
#endif



#endif

