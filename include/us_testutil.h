#ifndef US_TESTUTIL_H
#define US_TESTUTIL_H

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

#include "us_buffer.h"

#include "us_logger.h"

/**
 \addtogroup us_testutil  Test Utilities
 */
/*@{*/

#if US_ENABLE_PRINTING
#include "us_print.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef US_TESTUTIL_PRINTBUFFER_SIZE
#define US_TESTUTIL_PRINTBUFFER_SIZE ( 8192 )
#endif

#ifndef US_TESTUTIL_BUFFER_SIZE_IN_WORDS
#define US_TESTUTIL_BUFFER_SIZE_IN_WORDS ( 4096 )
#endif

bool us_testutil_start( int32_t sys_allocator_size, int32_t session_allocator_size, int argc, const char **argv );

void us_testutil_finish( void );

extern us_allocator_t *us_testutil_sys_allocator;
extern us_allocator_t *us_testutil_session_allocator;

#if US_ENABLE_PRINTING
extern us_print_t *us_testutil_printer_stdout;
extern us_print_t *us_testutil_printer_stderr;

#if US_ENABLE_STDIO
extern us_print_file_t us_testutil_printer_stdout_impl;
extern us_print_file_t us_testutil_printer_stderr_impl;
#else
extern us_printraw_t us_testutil_printer_stdout_impl;
extern us_printraw_t us_testutil_printer_stderr_impl;
extern char us_testutil_printbuffer_stdout[US_TESTUTIL_PRINTBUFFER_SIZE];
extern char us_testutil_printbuffer_stderr[US_TESTUTIL_PRINTBUFFER_SIZE];
#endif
#endif

#if US_ENABLE_MALLOC
extern us_malloc_allocator_t us_testutil_sys_allocator_impl;
#else
extern us_simple_allocator_t us_testutil_sys_allocator_impl;

extern int32_t us_testutil_sys_buffer[US_TESTUTIL_BUFFER_SIZE_IN_WORDS];
#endif
extern us_simple_allocator_t us_testutil_session_allocator_impl;
extern int32_t us_testutil_session_buffer[US_TESTUTIL_BUFFER_SIZE_IN_WORDS];

/*@}*/
#ifdef __cplusplus
}
#endif

#endif
