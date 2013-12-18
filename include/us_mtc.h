#ifndef US_MTC_H
#define US_MTC_H

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
#include "us_midi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup us_mtc
*/
/*@{*/
typedef enum us_mtc_format_e {
    US_MTC_RATE_24 = 0,
    US_MTC_RATE_25,
    US_MTC_RATE_2997,
    US_MTC_RATE_2997DF,
    US_MTC_RATE_30,
    US_MTC_RATE_30DF,
    US_MTC_RATE_2398,
    US_MTC_RATE_2398DF,
    US_MTC_RATE_UNKNOWN,
    US_MTC_NUM_RATES
} us_mtc_format_t;

extern const uint8_t us_mtc_max_frame[];
extern const bool us_mtc_is_drop[];
extern const uint8_t us_mtc_typecode[];
extern const char *us_mtc_format_string[];
extern const us_mtc_format_t us_mtc_fmt_from_midi[];

typedef struct us_mtc_s {
    us_mtc_format_t m_fmt;
    uint8_t m_frame;
    uint8_t m_second;
    uint8_t m_minute;
    uint8_t m_hour;
} us_mtc_t;

/**
   initialize mtc structure to 00:00:00:00 30FPS
 */
void us_mtc_init(us_mtc_t *self);

/**
  Return True if the time code is valid.
  Invalid time code is any hour/minute/second/frame out of range as well
  as a time that does not exist in drop frame mode like 00:01:00;00
*/
bool us_mtc_valid(us_mtc_t *self);

/**
Increment the time to the next frame, skipping any drop frames when necessary.
*/
void us_mtc_increment(us_mtc_t *self);

/**
Decrement the time to the previous frame, skipping any drop frames when necessary.
*/
void us_mtc_decrement(us_mtc_t *self);

/**
Compare two times, 'left' and 'right'
returns:
  - -1 if left < right
  - 0 if left == right
  - +1 if left > right
*/
int us_mtc_compare(us_mtc_t *left, us_mtc_t *right);

/**
Print the time code to a buffer.
if print_fmt is true then it appends the format description.
returns false if the buffer isn't big enough.
If the format is a drop frame format, then the time code time
will be in the form:
  - "HH:MM:SS;FF"
If the format is a non drop frame format then the time code time
will be in the form:
  - "HH:MM:SS:FF"
*/
bool us_mtc_print(us_mtc_t *self, bool print_fmt, char *buf, size_t buf_len);

/**
Parse the ascii as a time code time passed as ascii text.
fmt must be set to the time code format to expect.
If the format is a drop frame format, then the time code time
must be in the form:
  - "HH:MM:SS;FF"
If the format is a non drop frame format then the time code time
must be in the form:
  - "HH:MM:SS:FF"
*/
bool us_mtc_parse(us_mtc_t *self, const char *ascii, us_mtc_format_t fmt);
int32_t us_mtc_get_total_frames(us_mtc_t *self);
void us_mtc_set_total_frames(us_mtc_t *self, int32_t tf);
int us_mtc_extract_qf(us_mtc_t *self, int qf);
bool us_mtc_store_qf(us_mtc_t *self, int qf, int qf_value);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
