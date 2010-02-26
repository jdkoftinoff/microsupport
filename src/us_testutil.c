#include "us_world.h"

#include "us_testutil.h"

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

us_allocator_t *us_testutil_sys_allocator;
us_allocator_t *us_testutil_session_allocator;

#if US_ENABLE_PRINTING
us_print_t *us_testutil_printer_stdout;
us_print_t *us_testutil_printer_stderr;

#if US_ENABLE_STDIO
us_print_file_t us_testutil_printer_stdout_impl;
us_print_file_t us_testutil_printer_stderr_impl;
#else
us_printbuf_t us_testutil_printer_stdout_impl;
us_printbuf_t us_testutil_printer_stderr_impl;
char us_testutil_printbuffer_stdout[ US_TESTUTIL_PRINTBUFFER_SIZE ];
char us_testutil_printbuffer_stderr[ US_TESTUTIL_PRINTBUFFER_SIZE ];
#endif
#endif

#if !US_ENABLE_MALLOC
int32_t us_testutil_sys_buffer[ US_TESTUTIL_BUFFER_SIZE_IN_WORDS ];
int32_t us_testutil_session_sys_buffer[ US_TESTUTIL_BUFFER_SIZE_IN_WORDS ];
#endif

us_allocator_t us_testutil_sys_allocator_impl;
us_allocator_t us_testutil_session_allocator_impl;


bool us_testutil_start(
                         int32_t sys_allocator_size,
                         int32_t session_allocator_size,
                         int argc,
                         char **argv
                         )
{
  bool r=true;
  (void)argc;
  (void)argv;
  /*
    On windows, initialize winsock
  */

#ifdef WIN32
  r = us_platform_init_winsock();

  if( !r )
  {
    return r;
  }

#endif


  /*
    Initialize the system and the session allocators
  */

  us_testutil_sys_allocator = 0;
  us_testutil_session_allocator = 0;

#if US_ENABLE_MALLOC

  /*
    With malloc if we have it
  */

  us_testutil_sys_allocator=
    us_malloc_allocator_init(
                               &us_testutil_sys_allocator_impl,
                               (int32_t)(sys_allocator_size * sizeof(int32_t))
                               );

  us_testutil_session_allocator=
    us_malloc_allocator_init(
                               &us_testutil_session_allocator_impl,
                               (int32_t)(session_allocator_size * sizeof(int32_t))
                               );
#else

  (void)sys_allocator_size;
  (void)session_allocator_size;

  /*
    or with statically allocated bss segment data if we don't have malloc
  */
  us_testutil_sys_allocator=
    us_simple_allocator_init(
                               &us_testutil_sys_allocator_impl,
                               &us_testutil_session_sys_buffer,
                               (int32_t)(US_TESTUTIL_BUFFER_SIZE_IN_WORDS * sizeof(int32_t))
                               );


  us_testutil_session_allocator=
    us_simple_allocator_init(
                               &us_testutil_session_allocator_impl,
                               &us_testutil_session_sys_buffer,
                               (int32_t)(US_TESTUTIL_BUFFER_SIZE_IN_WORDS * sizeof(int32_t))
                               );
#endif

  /*
    check for allocation failure
  */
  if( !us_testutil_sys_allocator || !us_testutil_session_allocator )
  {
    r=false;
  }

  /*
    Initialize any printing mechanism if enabled
  */
#if US_ENABLE_PRINTING
#if US_ENABLE_STDIO

  /*
    Using stdio.h FILE stdout if available
  */

  us_testutil_printer_stdout =
    us_print_file_init(
                         &us_testutil_printer_stdout_impl,
                         stdout
                         );

  us_testutil_printer_stderr =
    us_print_file_init(
                         &us_testutil_printer_stderr_impl,
                         stderr
                         );


#else

  /*
    Using statically allocated character buffer if not
  */
  us_testutil_printer_stdout =
    us_printbuf_init(
                       &us_testutil_printer_stdout_impl,
                       us_testutil_printbuffer,
                       US_TESTUTIL_PRINTBUFFER_SIZE
                       );

  us_testutil_printer_stderr =
    us_printbuf_init(
                       &us_testutil_printer_stderr_impl,
                       us_testutil_printbuffer,
                       US_TESTUTIL_PRINTBUFFER_SIZE
                       );

#endif

  /*
    report any failure of the printer
  */
  if( !us_testutil_printer_stdout || !us_testutil_printer_stderr )
  {
    r=false;
  }

  us_stdout = us_testutil_printer_stdout;
  us_stderr = us_testutil_printer_stderr;

#endif

  return r;
}

void us_testutil_finish(void)
{
#if US_ENABLE_PRINTING

  /*
    destroy/delete/deallocate/close any printer if printing is enabled
  */
  if( us_testutil_printer_stdout )
  {
    us_testutil_printer_stdout->destroy( us_testutil_printer_stdout );
  }

  if( us_testutil_printer_stderr )
  {
    us_testutil_printer_stderr->destroy( us_testutil_printer_stderr );
  }

#endif
  /*
    deallocate any system of session allocators
  */
  if( us_testutil_sys_allocator )
  {
    us_testutil_sys_allocator->destroy( us_testutil_sys_allocator );
  }
  if( us_testutil_session_allocator )
  {
    us_testutil_session_allocator->destroy( us_testutil_session_allocator );
  }
}

