#ifndef US_WORLD_H
#define US_WORLD_H

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
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/**
 \addtogroup US_world Platform Specific Configuration
 */
/*@{*/

#ifdef US_FIRST_HEADER
# include US_FIRST_HEADER
#endif

#if defined(TARGET_PLATFORM_POSIX)
# define US_CONFIG_POSIX
#elif defined(TARGET_PLATFORM_LINUX)
# define US_CONFIG_POSIX
# define US_CONFIG_LINUX
#endif

#if defined(__APPLE__)
# undef US_CONFIG_MACOSX
# define US_CONFIG_MACOSX
# undef US_CONFIG_POSIX
# define US_CONFIG_POSIX
#elif defined(__linux__)
# undef US_CONFIG_LINUX
# define US_CONFIG_LINUX
# undef US_CONFIG_POSIX
# define US_CONFIG_POSIX
#elif defined(__CYGWIN__)
# undef US_CONFIG_POSIX
# define US_CONFIG_POSIX
#elif defined(WIN32) || defined(_WIN32) || defined( _MSC_VER )
# undef US_CONFIG_WIN32
# define US_CONFIG_WIN32
#ifndef _CRT_SECURE_NO_DEPRECATE
# define _CRT_SECURE_NO_DEPRECATE 1
#endif
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0600
# undef WINVER
# define WINVER _WIN32_WINNT
#endif
#ifndef _SCL_SECURE_NO_DEPRECATE
# define _SCL_SECURE_NO_DEPRECATE 1
#endif
#endif

#include "us_options.h"

#if defined( _MSC_VER )
# pragma warning( disable : 4267 )
#endif

#if defined(US_CONFIG_WIN32)
#ifndef inline
#define inline __inline
#endif


# include <winsock2.h>
# include <ws2tcpip.h>
# include <mswsock.h>
# ifndef EADDRINUSE
#  define EADDRINUSE 112  /* Address already in use */
# endif

# include <windows.h>
# include <io.h>
# include <process.h>

typedef long ssize_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef unsigned char uint8_t;

#endif

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <stddef.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <stdarg.h>

#if defined(US_CONFIG_POSIX)
# include <sys/types.h>
# include <unistd.h>
# include <pthread.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <unistd.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/poll.h>
# include <errno.h>
#endif

#if defined(_WIN32)
# include <io.h>
#endif

#include "us_platform.h"

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
