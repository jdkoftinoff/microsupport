#include "us_world.h"
#include "us_print.h"
#include "us_logger_stdio.h"

#include "us_print.h"
#include "us_logger_syslog.h"

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

#if US_ENABLE_STDIO

bool us_logger_stdio_files = false;
FILE *us_logger_stdio_out = 0;
FILE *us_logger_stdio_err = 0;

bool us_logger_stdio_start ( FILE *outfile, FILE *errfile )
{
    us_logger_stdio_files = false;
    us_logger_stdio_out = outfile;
    us_logger_stdio_err = errfile;
    us_log_error_proc = us_log_error_stdio;
    us_log_warn_proc = us_log_warn_stdio;
    us_log_info_proc = us_log_info_stdio;
    us_log_debug_proc = us_log_debug_stdio;
    us_log_trace_proc = us_log_trace_stdio;
    us_logger_finish = us_logger_stdio_finish;
    return true;
}

bool us_logger_init_stdio_files ( const char *outfilename, const char *errfilename )
{
    bool r = false;
    us_logger_stdio_out = fopen ( outfilename, "at" );
    if ( us_logger_stdio_out )
    {
        us_logger_stdio_err = fopen ( errfilename, "at" );
        if ( us_logger_stdio_err )
        {
            us_logger_stdio_files = true;
            us_log_error_proc = us_log_error_stdio;
            us_log_warn_proc = us_log_warn_stdio;
            us_log_info_proc = us_log_info_stdio;
            us_log_debug_proc = us_log_debug_stdio;
            us_logger_finish = us_logger_stdio_finish;
        }
        else
        {
            fclose ( us_logger_stdio_out );
        }
    }
    return r;
}

void us_logger_stdio_finish ( void )
{
    if ( us_logger_stdio_files )
    {
        if ( us_logger_stdio_out )
        {
            fflush ( us_logger_stdio_out );
            fclose ( us_logger_stdio_out );
            us_logger_stdio_out = 0;
        }
        if ( us_logger_stdio_err )
        {
            fflush ( us_logger_stdio_err );
            fclose ( us_logger_stdio_err );
            us_logger_stdio_err = 0;
        }
    }
    us_log_error_proc = us_log_null;
    us_log_warn_proc = us_log_null;
    us_log_info_proc = us_log_null;
    us_log_debug_proc = us_log_null;
    us_logger_finish = us_logger_null_finish;
}

void us_log_error_stdio ( const char *fmt, ... )
{
    struct timeval tv;
    va_list ap;
    va_start ( ap, fmt );
    us_gettimeofday( &tv );
    US_DEFAULT_FPRINTF ( stderr, "ERROR (@%ld.%ld):\t", (long)tv.tv_sec, (long)tv.tv_usec );
    US_DEFAULT_VFPRINTF ( stderr, fmt, ap );
    US_DEFAULT_FPRINTF ( stderr, "\n" );
    va_end ( ap );
    fflush( stderr );
}

void us_log_warn_stdio ( const char *fmt, ... )
{
    struct timeval tv;
    va_list ap;
    va_start ( ap, fmt );
    us_gettimeofday( &tv );
    US_DEFAULT_FPRINTF ( stderr, "WARNING (@%ld.%ld):\t", (long)tv.tv_sec, (long)tv.tv_usec );
    US_DEFAULT_VFPRINTF ( stderr, fmt, ap );
    US_DEFAULT_FPRINTF ( stderr, "\n" );
    va_end ( ap );
    fflush( stderr );
}

void us_log_info_stdio ( const char *fmt, ... )
{
    struct timeval tv;
    va_list ap;
    va_start ( ap, fmt );
    us_gettimeofday( &tv );
    US_DEFAULT_FPRINTF ( stderr, "INFO (@%ld.%ld):\t", (long)tv.tv_sec, (long)tv.tv_usec );
    US_DEFAULT_VFPRINTF ( stdout, fmt, ap );
    US_DEFAULT_FPRINTF ( stdout, "\n" );
    va_end ( ap );
    fflush( stdout );
}

void us_log_debug_stdio ( const char *fmt, ... )
{
    struct timeval tv;
    va_list ap;
    va_start ( ap, fmt );
    us_gettimeofday( &tv );
    US_DEFAULT_FPRINTF ( stderr, "DEBUG (@%ld.%ld):\t", (long)tv.tv_sec, (long)tv.tv_usec );
    US_DEFAULT_VFPRINTF ( stdout, fmt, ap );
    US_DEFAULT_FPRINTF ( stdout, "\n" );
    va_end ( ap );
    fflush( stdout );
}

void us_log_trace_stdio ( const char *fmt, ... )
{
    struct timeval tv;
    va_list ap;
    va_start ( ap, fmt );
    us_gettimeofday( &tv );
    US_DEFAULT_FPRINTF ( stderr, "TRACE (@%ld.%ld):\t", (long)tv.tv_sec, (long)tv.tv_usec );
    US_DEFAULT_VFPRINTF ( stdout, fmt, ap );
    US_DEFAULT_FPRINTF ( stdout, "\n" );
    va_end ( ap );
    fflush( stdout );
}

#endif

