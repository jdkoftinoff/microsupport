#ifndef US_OPTIONS_H
#define US_OPTIONS_H

#include "us_world.h"

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

/**
 \addtogroup us_options  Compile Time Options
 */
/*@{*/


#ifdef US_CUSTOM_OPTIONS_HEADER
# include US_CUSTOM_OPTIONS_HEADER
#endif

#ifdef US_CONFIG_MICROCONTROLLER
#endif

#ifndef US_ENABLE_UIPV4_STACK
# define US_ENABLE_UIPV4_STACK (0)
#endif

#ifndef US_ENABLE_LWIP_STACK
# define US_ENABLE_LWIP_STACK (0)
#endif

#ifndef US_ENABLE_NETWORK
# define US_ENABLE_NETWORK (1)
#endif


#if defined( US_ENABLE_NETWORK ) && defined(US_CONFIG_POSIX)
# define US_ENABLE_NETWORK_IPV4 (1)
# define US_ENABLE_NETWORK_IPV6 (1)
# define US_ENABLE_BSD_SOCKETS (1)
#elif defined( US_ENABLE_NETWORK ) && defined(US_ENABLE_UIPV4_STACK)
# define US_ENABLE_NETWORK_IPV4 (1)
# define US_ENABLE_NETWORK_IPV6 (0)
# define US_ENABLE_BSD_SOCKETS (0)
#elif defined( US_ENABLE_NETWORK ) && defined(US_ENABLE_UIPV6_STACK)
# define US_ENABLE_NETWORK_IPV4 (1)
# define US_ENABLE_NETWORK_IPV6 (1)
# define US_ENABLE_BSD_SOCKETS (0)
#elif defined( US_ENABLE_NETWORK ) && defined(US_ENABLE_LWIP_STACK)
# define US_ENABLE_NETWORK_IPV4 (1)
# define US_ENABLE_NETWORK_IPV6 (0)
# define US_ENABLE_BSD_SOCKETS (0)
#elif defined( US_ENABLE_NETWORK ) && defined(US_CONFIG_WIN32)
# define US_ENABLE_NETWORK_IPV4 (1)
# define US_ENABLE_NETWORK_IPV6 (1)
# define US_ENABLE_BSD_SOCKETS (1)
#endif

#if US_ENABLE_RAW_ETHERNET
#if defined(US_CONFIG_MACOSX) || defined(US_CONFIG_WIN32)
# undef US_ENABLE_PCAP
# define US_ENABLE_PCAP 1
# undef US_ENABLE_RAW_ETHERNET
# define US_ENABLE_RAW_ETHERNET (1)
#endif
#endif

#ifndef US_ENABLE_RAW_ETHERNET
#if defined(US_CONFIG_LINUX) || defined(__linux__)
# define US_ENABLE_RAW_ETHERNET (1)
#endif
#endif


#ifndef US_MAX_ADDRESS_LENGTH
# define US_MAX_ADDRESS_LENGTH (256)
#endif

#ifndef US_MAX_TYPETAGS
# define US_MAX_TYPETAGS (16)
#endif

#ifndef US_ENABLE_MALLOC
# define US_ENABLE_MALLOC (1)
#endif

#ifndef US_ENABLE_FLOAT
# define US_ENABLE_FLOAT (1)
#endif

#ifndef US_ENABLE_DOUBLE
# define US_ENABLE_DOUBLE US_ENABLE_FLOAT
#endif

#ifndef US_ENABLE_PRINTING
# define US_ENABLE_PRINTING (1)
#endif

#ifndef US_ENABLE_LOGGING
# define US_ENABLE_LOGGING (1)
#endif

#ifndef US_ENABLE_SYSLOG
# define US_ENABLE_SYSLOG (0)
#endif

#ifndef US_ENABLE_STDIO
# if defined(US_CONFIG_MICROCONTROLLER)
#  define US_ENABLE_STDIO (0)
# else
#  define US_ENABLE_STDIO (1)
# endif
#endif

#ifndef US_DEFAULT_MALLOC
# define US_DEFAULT_MALLOC malloc
#endif

#ifndef US_DEFAULT_REALLOC
# define US_DEFAULT_REALLOC realloc
#endif

#ifndef US_DEFAULT_FREE
# define US_DEFAULT_FREE free
#endif

#ifndef US_DEFAULT_PRINTF
# define US_DEFAULT_PRINTF printf
#endif

#ifndef US_DEFAULT_FPRINTF
# define US_DEFAULT_FPRINTF fprintf
#endif

#ifndef US_DEFAULT_SNPRINTF
# ifdef _MSC_VER
#  define US_DEFAULT_SNPRINTF sprintf_s
# else
#  define US_DEFAULT_SNPRINTF snprintf
# endif
#endif

#ifndef US_DEFAULT_VFPRINTF
# define US_DEFAULT_VFPRINTF vfprintf
#endif

#ifndef US_DEFAULT_VSNPRINTF
# define US_DEFAULT_VSNPRINTF vsnprintf
#endif

#ifndef US_ENABLE_LOGGING
# define US_ENABLE_LOGGING 1
#endif

/*@}*/

#endif
