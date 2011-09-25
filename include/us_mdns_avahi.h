#ifndef US_MDNS_AVAHI_H
#define US_MDNS_AVAHI_H


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
#include "us_allocator.h"
#ifdef __cplusplus
extern "C" {
#endif
    

#if US_ENABLE_STDIO

    typedef struct us_mdns_txt_record_s
    {
        const char *m_key;
        const char *m_value;
        struct us_mdns_txt_record_s *m_next;
    } us_mdns_txt_record_t;

    typedef struct us_mdns_avahi_s
    {
        const char *m_name;
        const char *m_type;
        int16_t m_port;
        const char *m_fname;
        const us_mdns_txt_record_t *m_txt;
        struct us_mdns_avahi_s *m_next;
    } us_mdns_avahi_t;

    bool us_mdns_avahi_create_service_file( us_mdns_avahi_t *description );
    void us_mdns_avahi_atexit(void);

    bool us_mdns_avahi_write_xml(
        const char *avahi_services_dir,
        const char *fname,
        const char *service_name,
        const char *udp_service_type,
        const char *udp_port,
        const char *tcp_service_type,
        const char *tcp_port
    );
    
#endif
#ifdef __cplusplus
}
#endif


#endif

