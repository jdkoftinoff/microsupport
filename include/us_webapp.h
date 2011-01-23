#ifndef US_WEBAPP_H
#define US_WEBAPP_H


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

#include "us_http_server.h"

#ifdef __cplusplus
extern "C"
{
#endif


    typedef struct us_webapp_s
    {
        us_allocator_t *m_allocator;
        void (*destroy)(
            struct us_webapp_s *self
        );
        bool (*path_match)(
            struct us_webapp_s *self,
            const char *path
        );
        int (*dispatch)(
            struct us_webapp_s *self,
            const us_http_request_header_t *request_header,
            const us_buffer_t *request_content,
            us_http_response_header_t *response_header,
            us_buffer_t *response_content
        );
        struct us_webapp_s *m_next;
    } us_webapp_t;

    bool us_webapp_init( us_webapp_t *self, us_allocator_t *allocator );
    void us_webapp_destroy( us_webapp_t *self );
    bool us_webapp_path_match(us_webapp_t *self, const char *url );
    int us_webapp_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );

    typedef struct us_webapp_static_buffer_s
    {
        us_webapp_t m_base;
        char *m_path;
        char *m_content_type;
        us_buffer_t *m_buffer;
        bool m_owned_buffer;
    } us_webapp_static_buffer_t;

    us_webapp_static_buffer_t *us_webapp_static_buffer_create(
        us_allocator_t *allocator
    );

    us_webapp_static_buffer_t *us_webapp_static_buffer_create_with_buffer(
        us_allocator_t *allocator,
        const char *path,
        const char *content_type,
        us_buffer_t *buffer,
        bool own_buffer
    );

    us_webapp_static_buffer_t *us_webapp_static_buffer_create_with_string(
        us_allocator_t *allocator,
        const char *path,
        const char *content_type,
        const char *str
    );

    void us_webapp_static_buffer_destroy(
        us_webapp_t *self
    );

    bool us_webapp_static_buffer_path_match(us_webapp_t *self, const char *url );
    int us_webapp_static_buffer_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );

    typedef struct us_webapp_redirect_s
    {
        us_webapp_t m_base;
        const char *m_original_path;
        const char *m_new_path;
        int m_redirect_code;
    } us_webapp_redirect_t;

    us_webapp_redirect_t *us_webapp_redirect_create(
        us_allocator_t *allocator,
        const char *original_path,
        const char *new_path,
        int redirect_code
    );

    void us_webapp_redirect_destroy(
        us_webapp_t *self
    );

    bool us_webapp_redirect_path_match(us_webapp_t *self, const char *url );

    int us_webapp_redirect_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );



    typedef struct us_webapp_diag_s
    {
        us_webapp_t m_base;
    } us_webapp_diag_t;

    us_webapp_diag_t *us_webapp_diag_create(
        us_allocator_t *allocator
    );

    void us_webapp_diag_destroy(
        us_webapp_t *self
    );

    bool us_webapp_diag_path_match(us_webapp_t *self, const char *url );
    int us_webapp_diag_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );


    typedef struct us_webapp_director_s
    {
        us_allocator_t *m_allocator;
        void (*destroy)( struct us_webapp_director_s *self );
        int (*dispatch)(
            struct us_webapp_director_s *self,
            const us_http_request_header_t *request_header,
            const us_buffer_t *request_content,
            us_http_response_header_t *response_header,
            us_buffer_t *response_content
        );
        us_webapp_t *m_apps;
        us_webapp_t *m_last_app;
        us_webapp_t *m_404_app;
    } us_webapp_director_t;


    bool us_webapp_director_init( us_webapp_director_t *self, us_allocator_t *allocator );

    void us_webapp_director_destroy( us_webapp_director_t *self );

    bool us_webapp_director_add_app( us_webapp_director_t *self, us_webapp_t *m_app );

    bool us_webapp_director_add_404_app( us_webapp_director_t *self, us_webapp_t *m_app );


    int us_webapp_director_dispatch(
        us_webapp_director_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );


#ifdef __cplusplus
}
#endif



#endif

