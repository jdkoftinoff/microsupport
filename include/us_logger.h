#ifndef US_LOGGER_H
#define US_LOGGER_H

/*
Copyright (c) 2013, J.D. Koftinoff Software, Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     \addtogroup us_logger Logger
     */
    /*@{*/

# define US_LOG_LEVEL_NONE  (0)
# define US_LOG_LEVEL_ERROR (1)
# define US_LOG_LEVEL_WARN  (2)
# define US_LOG_LEVEL_INFO  (3)
# define US_LOG_LEVEL_DEBUG (4)
# define US_LOG_LEVEL_TRACE (5)

# if US_ENABLE_LOGGING
#  ifndef US_ENABLE_LOG_ERROR
#   define US_ENABLE_LOG_ERROR (1)
#  endif
#  ifndef US_ENABLE_LOG_WARN
#   define US_ENABLE_LOG_WARN (1)
#  endif
#  ifndef US_ENABLE_LOG_INFO
#   define US_ENABLE_LOG_INFO (1)
#  endif
#  ifndef US_ENABLE_LOG_DEBUG
#   define US_ENABLE_LOG_DEBUG (1)
#  endif
#  ifndef US_ENABLE_LOG_TRACE
#   define US_ENABLE_LOG_TRACE (1)
#  endif
# endif

    extern int us_log_level;
    extern void ( *us_log_error_proc ) ( const char *fmt, ... );
    extern void ( *us_log_warn_proc ) ( const char *fmt, ... );
    extern void ( *us_log_info_proc ) ( const char *fmt, ... );
    extern void ( *us_log_debug_proc ) ( const char *fmt, ... );
    extern void ( *us_log_trace_proc ) ( const char *fmt, ... );
    extern void ( *us_logger_finish ) ( void );

    void us_log_null ( const char *fmt, ... );
    void us_logger_null_finish ( void );



# if US_ENABLE_LOGGING

#  define us_log_set_level(L) do { us_log_level=(L); } while(0)

#  if US_ENABLE_LOG_ERROR
#   define us_log_error(...) do { if( us_log_level>=US_LOG_LEVEL_ERROR ) us_log_error_proc( __VA_ARGS__ ); } while(0)
#  else
#   define us_log_error(...) do { } while(0)
#  endif

#  if US_ENABLE_LOG_WARN
#   define us_log_warn(...) do { if( us_log_level>=US_LOG_LEVEL_WARN ) us_log_warn_proc( __VA_ARGS__ ); } while(0)
#  else
#   define us_log_warn(...) do { } while(0)
#  endif

#  if US_ENABLE_LOG_INFO
#   define us_log_info(...) do { if( us_log_level>=US_LOG_LEVEL_INFO ) us_log_info_proc( __VA_ARGS__ ); } while(0)
#  else
#   define us_log_info(...) do { } while(0)
#  endif

#if US_ENABLE_LOG_DEBUG
#  define us_log_debug(...) do { if( us_log_level>=US_LOG_LEVEL_DEBUG ) us_log_debug_proc( __VA_ARGS__ ); } while(0)
# else
#  define us_log_debug(...) do { } while(0)
# endif

#if US_ENABLE_LOG_TRACE
#  define us_log_trace(...) do { if( us_log_level>=US_LOG_LEVEL_TRACE ) us_log_trace_proc( __VA_ARGS__ ); } while(0)
# else
#  define us_log_trace(...) do { } while(0)
# endif

#ifndef us_log_tracepoint
# define us_log_tracepoint() us_log_trace( "At %s:%d %s", __FILE__, __LINE__, __FUNCTION__ )
#endif

# else

#  define us_log_set_level(L) do { } while(0)
#  define us_log_error(...) do { } while(0)
#  define us_log_warn(...) do { } while(0)
#  define us_log_info(...) do { } while(0)
#  define us_log_debug(...) do { } while(0)
#  define us_log_trace(...) do {} while(0)

#endif

# define us_log_probe() us_log_debug( "At '%s':%d function '%s'", __FILE__, __LINE__, __FUNCTION__ )


    /*@}*/
#ifdef __cplusplus
}
#endif

#endif
