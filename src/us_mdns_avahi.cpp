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

#include "us_world.hpp"

#include "us_mdns_avahi.hpp"


#if US_ENABLE_STDIO && !defined(WIN32)

static us_mdns_avahi_t *us_mdns_avahi_service_list=0;

bool us_mdns_avahi_create_service_file( us_mdns_avahi_t *description )
{
    bool r=false;
    FILE *f = fopen( description->m_fname, "wt" );
    if (f)
    {
        const us_mdns_txt_record_t *curtxt = description->m_txt;
        fprintf( f,
                 "<?xml version=\"1.0\" standalone='no'?>\n"
                 "<!DOCTYPE service-group SYSTEM \"avahi-service.dtd\">\n"
                 " <service-group>\n"
                 "  <name replace-wildcards=\"yes\">%%h %s</name>\n"
                 "  <service>\n"
                 "   <type>%s</type>\n"
                 "   <port>%d</port>\n",
                 description->m_name,
                 description->m_type,
                 description->m_port
               );
        while ( curtxt )
        {
            fprintf( f, "   <txt-record>%s=%s</txt-record>\n", curtxt->m_key, curtxt->m_value );
            curtxt = curtxt->m_next;
        }
        fprintf( f,
                 " </service>\n"
                 "</service-group>\n"
               );
        fflush(f);
        if ( !ferror(f) )
        {
            us_mdns_avahi_t *cur = us_mdns_avahi_service_list;
            description->m_next = 0;
            if ( cur==0 )
            {
                us_mdns_avahi_service_list = description;
                atexit( us_mdns_avahi_atexit );
            }
            else
            {
                while ( cur->m_next )
                {
                    cur=cur->m_next;
                }
                cur->m_next = description;
            }
            r=true;
        }
        fclose(f);
    }
    return r;
}

void us_mdns_avahi_atexit(void)
{
    us_mdns_avahi_t *cur = us_mdns_avahi_service_list;
    while ( cur )
    {
       unlink( cur->m_fname );
    }
}

#endif