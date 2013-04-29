#include "us_world.h"
#include "us_print.h"
#include "us_logger_printer.h"
#include "us_print.h"
#include "us_logger_syslog.h"

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

#if US_ENABLE_PRINTING

us_print_t *us_logger_printer_out = 0;
us_print_t *us_logger_printer_err = 0;

bool us_logger_printer_start ( us_print_t *out_printer, us_print_t *err_printer )
{
    us_logger_printer_out = out_printer;
    us_logger_printer_err = err_printer;
    us_log_error_proc = us_log_error_printer;
    us_log_warn_proc = us_log_warn_printer;
    us_log_info_proc = us_log_info_printer;
    us_log_debug_proc = us_log_debug_printer;
    us_log_trace_proc = us_log_trace_printer;
    us_logger_finish = us_logger_printer_finish;
    return true;
}

void us_logger_printer_finish ( void )
{
    us_log_error_proc = us_log_null;
    us_log_warn_proc = us_log_null;
    us_log_info_proc = us_log_null;
    us_log_debug_proc = us_log_null;
    us_logger_finish = us_logger_null_finish;
}

void us_log_error_printer ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    if ( us_logger_printer_err )
    {
        us_logger_printer_err->printf ( us_logger_printer_err, "ERROR:\t" );
        us_logger_printer_err->vprintf ( us_logger_printer_err, fmt, ap );
        us_logger_printer_err->printf ( us_logger_printer_err, "\n" );
    }
    va_end ( ap );
}

void us_log_warn_printer ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    if ( us_logger_printer_err )
    {
        us_logger_printer_err->printf ( us_logger_printer_err, "WARNING:\t" );
        us_logger_printer_err->vprintf ( us_logger_printer_err, fmt, ap );
        us_logger_printer_err->printf ( us_logger_printer_err, "\n" );
    }
    va_end ( ap );
}

void us_log_info_printer ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    if ( us_logger_printer_out )
    {
        us_logger_printer_out->printf ( us_logger_printer_out, "INFO:\t" );
        us_logger_printer_out->vprintf ( us_logger_printer_out, fmt, ap );
        us_logger_printer_out->printf ( us_logger_printer_out, "\n" );
    }
    va_end ( ap );
}

void us_log_debug_printer ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    if ( us_logger_printer_out )
    {
        us_logger_printer_out->printf ( us_logger_printer_out, "DEBUG:\t" );
        us_logger_printer_out->vprintf ( us_logger_printer_out, fmt, ap );
        us_logger_printer_out->printf ( us_logger_printer_out, "\n" );
    }
    va_end ( ap );
}

void us_log_trace_printer ( const char *fmt, ... )
{
    va_list ap;
    va_start ( ap, fmt );
    if ( us_logger_printer_out )
    {
        us_logger_printer_out->printf ( us_logger_printer_out, "TRACE:\t" );
        us_logger_printer_out->vprintf ( us_logger_printer_out, fmt, ap );
        us_logger_printer_out->printf ( us_logger_printer_out, "\n" );
    }
    va_end ( ap );
}

#endif

