#ifndef US_WEBAPP_FS_H
#define US_WEBAPP_FS_H


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
#include "us_webapp.h"
#ifdef __cplusplus
extern "C" {
#endif


    typedef struct us_webapp_file_ext_mime_map_s
    {
        const char *m_extension;
        const char *m_mime_type;
    } us_webapp_file_ext_mime_map_t;

#if US_ENABLE_STDIO

    typedef struct us_webapp_fs_s
    {
        us_webapp_t m_base;
        us_webapp_file_ext_mime_map_t *m_ext_map;
        const char *m_web_path_prefix;
        int32_t m_web_path_prefix_len;
        const char *m_filesystem_path;
    } us_webapp_fs_t;

    us_webapp_t *us_webapp_fs_create(
        us_allocator_t *allocator,
        us_webapp_file_ext_mime_map_t *ext_map,
        const char *web_path_prefix,
        const char *filesystem_path
    );

    void us_webapp_fs_destroy(
        us_webapp_t *self
    );

    bool us_webapp_fs_path_match(us_webapp_t *self, const char *path );
    int us_webapp_fs_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );

#endif

#ifdef __cplusplus
}
#endif

#endif

