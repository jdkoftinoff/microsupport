#include "us_world.h"
#include "us_queue.h"

#include "us_logger_printer.h"

#include "us_testutil.h"
#include "us_test_queue_main.h"

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
 * Neither the name of J.D. Koftinoff Software, Ltd. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD..  BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \addtogroup us_test_queue */
/*@{*/

static us_queue_t queue;
static uint8_t mem[8];

static bool us_validate_queue(us_queue_t *self, uint16_t expected_writable_count, uint16_t expected_readable_count);
static bool us_test_queue(void);

static bool us_validate_queue(us_queue_t *self, uint16_t expected_writable_count, uint16_t expected_readable_count) {
    bool r = true;
    uint16_t got_readable_count = us_queue_readable_count(self);
    uint16_t got_writable_count = us_queue_writable_count(self);
    bool expected_writable_byte = (expected_writable_count > 0);
    bool got_writable_byte = us_queue_can_write_byte(self);
    bool expected_readable_byte = (expected_readable_count > 0);
    bool got_readable_byte = us_queue_can_read_byte(self);
    if (got_readable_byte != expected_readable_byte) {
        us_log_error("Expected readable byte: %d got: %d", expected_readable_byte, got_readable_byte);
        r = false;
    }
    if (got_readable_count != expected_readable_count) {
        us_log_error("Expected readable count: %d got: %d", expected_readable_count, got_readable_count);
    }
    if (got_writable_byte != expected_writable_byte) {
        us_log_error("Expected writable byte: %d got: %d", expected_writable_byte, got_writable_byte);
        r = false;
    }
    if (got_writable_count != expected_writable_count) {
        us_log_error("Expected writable count: %d got: %d", expected_writable_count, got_writable_count);
    }
    us_log_debug("queue next_in=%d, next_out=%d", queue.m_next_in, queue.m_next_out);
    return r;
}

static bool us_test_queue(void) {
    bool r = true;
    uint16_t i;
    uint8_t v;
    uint8_t test_7_bytes[7] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22};
    uint8_t read_7_bytes[7] = {0, 0, 0, 0, 0, 0, 0};
    us_log_debug("init queue of 8 elements");
    us_queue_init(&queue, mem, 8);
    r &= us_validate_queue(&queue, 7, 0);
    us_log_debug("Try write 1 byte");
    if (us_queue_can_write_byte(&queue)) {
        us_queue_write_byte(&queue, 0x01);
    }
    r &= us_validate_queue(&queue, 6, 1);
    us_log_debug("Try write 1 byte");
    if (us_queue_can_write_byte(&queue)) {
        us_queue_write_byte(&queue, 0x05);
    }
    r &= us_validate_queue(&queue, 5, 2);
    us_log_debug("Try read first byte");
    if (us_queue_can_read_byte(&queue)) {
        v = us_queue_read_byte(&queue);
        if (v != 0x01) {
            us_log_error("Expected 0x01, got 0x%02x", v);
            r = false;
        }
    }
    r &= us_validate_queue(&queue, 6, 1);
    us_log_debug("Try read second byte");
    if (us_queue_can_read_byte(&queue)) {
        v = us_queue_read_byte(&queue);
        if (v != 0x05) {
            us_log_error("Expected 0x05, got 0x%02x", v);
            r = false;
        }
    }
    r &= us_validate_queue(&queue, 7, 0);
    us_log_debug("Try write 7 bytes");
    if (us_queue_writable_count(&queue) >= 7) {
        us_queue_write(&queue, test_7_bytes, sizeof(test_7_bytes));
    }
    r &= us_validate_queue(&queue, 0, 7);
    us_log_debug("Try peek 7th byte");
    v = us_queue_peek(&queue, 6);
    if (v != test_7_bytes[6]) {
        us_log_error("Expected 0x%02x, got 0x%02x", test_7_bytes[6], v);
        r = false;
    }
    us_log_debug("Try skip 7 bytes");
    us_queue_skip(&queue, 7);
    r &= us_validate_queue(&queue, 7, 0);
    us_log_debug("Try write 7 bytes");
    if (us_queue_writable_count(&queue) >= 7) {
        us_queue_write(&queue, test_7_bytes, sizeof(test_7_bytes));
    }
    r &= us_validate_queue(&queue, 0, 7);
    us_log_debug("Try read 7 bytes");
    us_queue_read(&queue, read_7_bytes, sizeof(read_7_bytes));
    for (i = 0; i < sizeof(read_7_bytes); ++i) {
        if (test_7_bytes[i] != read_7_bytes[i]) {
            us_log_error("Expected byte %d to be 0x%02x, got 0x%02x", i, test_7_bytes[i], read_7_bytes[i]);
            r = false;
        }
    }
    r &= us_validate_queue(&queue, 7, 0);
    return r;
}

int us_test_queue_main(int argc, const char **argv) {
    int r = 1;
    if (us_testutil_start(2048, 2048, argc, argv)) {
#if US_ENABLE_LOGGING
        us_logger_printer_start(us_testutil_printer_stdout, us_testutil_printer_stderr);
#endif
        us_log_set_level(US_LOG_LEVEL_DEBUG);
        us_log_info("Hello world from %s compiled on %s", __FILE__, __DATE__);
        if (us_test_queue())
            r = 0;
        us_log_info("Finishing us_test_buffer");
        us_logger_finish();
        us_testutil_finish();
    }
    return r;
}

/*@}*/
