#ifndef US_LOGGER_STDIO_H
#define US_LOGGER_STDIO_H

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
#include "us_logger.h"

/**
 \addtogroup us_logger Logger
 */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

  extern bool us_logger_stdio_files;
  extern FILE *us_logger_stdio_out;
  extern FILE *us_logger_stdio_err;

  bool us_logger_stdio_start( FILE *outfile, FILE *errfile );
  bool us_logger_stdio_start_files( const char *outfilename, const char *errfilename );
  void us_logger_stdio_finish(void);

  void us_log_error_stdio( const char *fmt, ... );
  void us_log_warn_stdio( const char *fmt, ... );
  void us_log_info_stdio( const char *fmt, ... );
  void us_log_debug_stdio( const char *fmt, ... );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
