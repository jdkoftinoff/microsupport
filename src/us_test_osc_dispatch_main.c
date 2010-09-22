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

enum
{
    US_TEST_MEDIA_IN,
    US_TEST_MEDIA_OUT
};

static const char *media_type[] = {"in", "out"};


static bool us_test_osc_dispatch_media_level(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
);
static bool us_test_osc_dispatch_media_pan(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
);
static bool us_test_osc_dispatch_media_mute(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
);
static bool us_test_osc_dispatch_media_invert(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
);

static bool us_test_osc_dispatch_media_raw(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
);

static us_osc_dispatch_table_t us_test_osc_dispatch_media_table[] =
{
    {
        "/raw/*",
        us_test_osc_dispatch_media_raw,
        {
            { 0, 0, 0, 0}
        }
    },
    {
        "/level",
        us_test_osc_dispatch_media_level,
        {
            { 0, 0, 0, 0}
        }
    },
    {
        "/pan",
        us_test_osc_dispatch_media_pan,
        {
            { 0, 0, 0, 0}
        }
    },
    {
        "/mute",
        us_test_osc_dispatch_media_mute,
        {
            { 0, 0, 0, 0}
        }
    },
    {
        "/invert",
        us_test_osc_dispatch_media_invert,
        {
            { 0, 0, 0, 0}
        }
    },
    {
        0,
        0,
        {
            { 0, 0, 0, 0}
        }
    }
};

static bool us_test_osc_dispatch_test1_setup(
    us_allocator_t *allocator,
    us_osc_dispatch_t *osc_dispatch
)
{
    bool r = false;
    if (us_osc_dispatch_init(osc_dispatch, allocator, 256, 2048))
    {
        char s[64];
        int mt, ch;
        us_osc_dispatch_index_t index;
        us_osc_dispatch_index_init(&index);
        r = true;
        for (mt = 0; mt < 2 && r == true; ++mt)
        {
            index.axis[0] = mt;
            for (ch = 0; ch < 8 && r == true; ++ch)
            {
                index.axis[1] = ch;
                sprintf(s, "/media/%s/%d", media_type[index.axis[0]], ch + 1);
                us_testutil_printer_stdout->printf(
                    us_testutil_printer_stdout,
                    "Adding address '%s'\n",
                    s
                );
                r &= us_osc_dispatch_add_table(
                         osc_dispatch,
                         s,
                         us_test_osc_dispatch_media_table,
                         &index
                     );
            }
        }
    }
    if (!r)
    {
        osc_dispatch->destroy(osc_dispatch);
    }
    return r;
}

static bool us_test_osc_dispatch_test1_feed(
    us_allocator_t *allocator,
    us_osc_dispatch_t *osc_dispatch
)
{
    bool r = false;
    us_osc_msg_t *msg;
    msg = us_osc_msg_form(
              allocator,
              "/media/in/2/level",
              ",f",
              10.0f
          );
    us_testutil_printer_stdout->printf(us_testutil_printer_stdout, "\n\nformed msg: \n");
    msg->print(msg, us_testutil_printer_stdout);
    osc_dispatch->receive_msg(osc_dispatch, msg, 0);
    msg->destroy(msg);
    msg = us_osc_msg_form(
              allocator,
              "/media/out/3/pan",
              ",f",
              -180.0f
          );
    us_testutil_printer_stdout->printf(us_testutil_printer_stdout, "\n\nformed msg: \n");
    msg->print(msg, us_testutil_printer_stdout);
    osc_dispatch->receive_msg(osc_dispatch, msg, 0);
    msg->destroy(msg);
    msg = us_osc_msg_form(
              allocator,
              "/media/in/2/mute",
              ",T"
          );
    us_testutil_printer_stdout->printf(us_testutil_printer_stdout, "\n\nformed msg: \n");
    msg->print(msg, us_testutil_printer_stdout);
    osc_dispatch->receive_msg(osc_dispatch, msg, 0);
    msg->destroy(msg);
    msg = us_osc_msg_form(
              allocator,
              "/media/out/raw/SOMETHING",
              ",T"
          );
    us_testutil_printer_stdout->printf(us_testutil_printer_stdout, "\n\nformed msg: \n");
    msg->print(msg, us_testutil_printer_stdout);
    osc_dispatch->receive_msg(osc_dispatch, msg, 0);
    msg->destroy(msg);
    msg = us_osc_msg_form(
              allocator,
              "/media/out/raw/OTHER",
              ",T"
          );
    us_testutil_printer_stdout->printf(us_testutil_printer_stdout, "\n\nformed msg: \n");
    msg->print(msg, us_testutil_printer_stdout);
    osc_dispatch->receive_msg(osc_dispatch, msg, 0);
    msg->destroy(msg);
    return r;
}

static bool us_test_osc_dispatch_test1(void)
{
    bool r = false;
    us_osc_dispatch_t osc_dispatch;
    us_log_info("us_test_osc_dispatch_test1 init");
    if (us_test_osc_dispatch_test1_setup(
                us_testutil_sys_allocator,
                &osc_dispatch
            ))
    {
        r = us_test_osc_dispatch_test1_feed(us_testutil_session_allocator, &osc_dispatch);
        osc_dispatch.destroy(&osc_dispatch);
    }
    return r;
}

static bool us_test_osc_dispatch_media_level(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
)
{
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "%s received:", __FUNCTION__
    );
    msg->print(msg, us_testutil_printer_stdout);
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "\nindex values: %d,%d,%d,%d\n",
        index->axis[0],
        index->axis[1],
        index->axis[2],
        index->axis[3]
    );
    return true;
}

static bool us_test_osc_dispatch_media_pan(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
)

{
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "%s received:", __FUNCTION__
    );
    msg->print(msg, us_testutil_printer_stdout);
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "\nindex values: %d,%d,%d,%d\n",
        index->axis[0],
        index->axis[1],
        index->axis[2],
        index->axis[3]
    );
    return true;
}

static bool us_test_osc_dispatch_media_mute(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
)
{
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "%s received:", __FUNCTION__
    );
    msg->print(msg, us_testutil_printer_stdout);
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "\nindex values: %d,%d,%d,%d\n",
        index->axis[0],
        index->axis[1],
        index->axis[2],
        index->axis[3]
    );
    return true;
}

static bool us_test_osc_dispatch_media_invert(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
)
{
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "%s received:", __FUNCTION__
    );
    msg->print(msg, us_testutil_printer_stdout);
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "\nindex values: %d,%d,%d,%d\n",
        index->axis[0],
        index->axis[1],
        index->axis[2],
        index->axis[3]
    );
    return true;
}

static bool us_test_osc_dispatch_media_raw(
    struct us_osc_dispatch_s *self,
    const us_osc_msg_t *msg,
    const us_osc_dispatch_index_t *index,
    void *extra
)
{
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "%s received:", __FUNCTION__
    );
    msg->print(msg, us_testutil_printer_stdout);
    us_testutil_printer_stdout->printf(
        us_testutil_printer_stdout,
        "\nindex values: %d,%d,%d,%d\n",
        index->axis[0],
        index->axis[1],
        index->axis[2],
        index->axis[3]
    );
    return true;
}

int us_test_osc_dispatch_main(int argc, char **argv)
{
    bool r = true;
    r = us_testutil_start(81920, 8192, argc, argv);
    if (r)
    {
#if US_ENABLE_LOGGING
        us_logger_printer_start(us_testutil_printer_stdout, us_testutil_printer_stderr);
#endif
        us_log_info("Hello world from %s compiled on %s", __FILE__, __DATE__);
        r &= us_test_osc_dispatch_test1();
        us_logger_finish();
        us_testutil_finish();
    }
    return r ? 0 : 1;
}

/*@}*/

