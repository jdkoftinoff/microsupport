#ifndef US_WORLD_H
#define US_WORLD_H

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



#if defined(US_CONFIG_POSIX)
# include <sys/types.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <string.h>
# include <unistd.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <stdarg.h>
# include <math.h>
# include <errno.h>
#endif

#if defined(_WIN32)
# include <stdio.h>
# include <math.h>
# include <io.h>
# include <fcntl.h>
# include <stdarg.h>
#endif

#ifdef BYTE_ORDER
# if BYTE_ORDER==BIG_ENDIAN
#  define US_BIG_ENDIAN (1)
# elif BYTE_ORDER==LITTLE_ENDIAN
#  define US_LITTLE_ENDIAN (1)
# endif
#endif

#ifndef AI_NUMERICSERV
# define AI_NUMERICSERV (0)
#endif

#define US_GET_BYTE_3(v) (uint8_t)((( (v) & 0xff000000) >> 24)&0xff)
#define US_GET_BYTE_2(v) (uint8_t)((( (v) & 0xff0000) >> 16)&0xff)
#define US_GET_BYTE_1(v) (uint8_t)((( (v) & 0xff00) >> 8)&0xff)
#define US_GET_BYTE_0(v) (uint8_t)((( (v) & 0xff) >> 0)&0xff)

#ifndef __cplusplus
# ifndef true
typedef int bool;
# define true (1)
# define false (0)
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef US_CONFIG_WIN32
  static inline void closesocket(int fd)
  {
    close(fd);
  }
#endif

#ifdef US_CONFIG_WIN32
  bool us_platform_init_winsock( void );
#endif


#ifdef US_CONFIG_WIN32
# if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#  define US_DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
# else
#  define US_DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
# endif

  struct timezone
  {
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime; /* type of dst correction */
  };

  int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

  void us_gettimeofday( struct timeval *tv );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
