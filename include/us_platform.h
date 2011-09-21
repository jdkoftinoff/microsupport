#ifndef US_PLATFORM_H
#define US_PLATFORM_H

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
   
/**
 \addtogroup us_platform  Platform Specific Configuration
 */
/*@{*/


#define US_GET_BYTE_3(v) (uint8_t)((( (v) & 0xff000000) >> 24)&0xff)
#define US_GET_BYTE_2(v) (uint8_t)((( (v) & 0xff0000) >> 16)&0xff)
#define US_GET_BYTE_1(v) (uint8_t)((( (v) & 0xff00) >> 8)&0xff)
#define US_GET_BYTE_0(v) (uint8_t)((( (v) & 0xff) >> 0)&0xff)

#define US_COUNTOF(x) (sizeof(x)/sizeof(x[0]))

#ifdef US_UNUSED
#elif defined(__GNUC__)
# define US_UNUSED(x) US_UNUSED_ ## x __attribute__((unused))
#else
# define US_UNUSED(x) US_UNUSED_ ## x
#endif

#if US_ENABLE_BSD_SOCKETS && !defined(US_NO_DECLARE_CLOSESOCKET)
# ifndef WIN32
    static inline int closesocket ( int fd )
    {
        return close ( fd );
    }
# endif

# ifdef WIN32
    bool us_platform_init_winsock ( void );
# endif

# ifndef WIN32
#  include <netdb.h>
#  include <unistd.h>
#  include <netinet/in.h>
# endif
#endif

#ifdef WIN32
# if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#  define US_DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
# else
#  define US_DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
# endif

#ifdef __cplusplus
extern "C" {
#endif
 
    struct timezone
    {
        int tz_minuteswest; /* minutes W of Greenwich */
        int tz_dsttime; /* type of dst correction */
    };

    int gettimeofday ( struct timeval *tv, struct timezone *tz );
#endif

#ifdef __AVR__
    struct timeval
    {
        int32_t tv_sec;
        int32_t tv_usec;
    };
#endif
    void us_gettimeofday ( struct timeval *tv );
    bool us_platform_init_sockets ( void );

    extern volatile char us_platform_sigterm_seen;
    extern volatile char us_platform_sigint_seen;
#ifdef __cplusplus
}
#endif

#if US_ENABLE_LWIP_STACK
# include "lwip/opt.h"
# include "lwip/arch.h"
# include "lwip/api.h"
#endif

/*@}*/

#endif
