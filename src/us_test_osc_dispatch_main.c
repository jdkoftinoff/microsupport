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
    * Neither the name of Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC.  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "us_world.h"
#include "us_testutil.h"
#include "us_logger_printer.h"

#include "us_buffer.h"
#include "us_osc_msg.h"
#include "us_osc_dispatch.h"

#if US_ENABLE_PRINTING
#include "us_osc_msg_print.h"
#include "us_buffer_print.h"
#endif

/** \addtogroup us_osc_msg_test_msg_dispatch */
/*@{*/


static bool us_test_osc_dispatch_test1( void )
{
    bool r=false;

    return r;
}


int us_test_osc_dispatch_main( int argc, char **argv )
{
    bool r=true;
    r=us_testutil_start(8192,8192,argc,argv);
    if ( r )
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start( us_testutil_printer_stdout, us_testutil_printer_stderr );
#endif
        us_log_info( "Hello world from %s compiled on %s", __FILE__, __DATE__ );

        r&=us_test_osc_dispatch_test1();

        us_logger_finish();
        us_testutil_finish();
    }
    return r ? 0:1;
}

/*@}*/

