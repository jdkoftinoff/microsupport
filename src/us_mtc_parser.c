#include "us_world.h"
#include "us_mtc_parser.h"
#include "us_net.h"

void us_mtc_parser_init(us_mtc_parser_t *self) {
    int i;
    for (i = 0; i < 8; ++i) {
        self->m_nibbles[i] = 0;
    }
    self->m_first_nibble = -1;
    self->m_last_nibble = -1;
    self->m_seen_first = false;
    us_mtc_init(&self->m_last_formed_time);
    us_gettimeofday(&self->m_timeout_timestamp);
    self->m_qf = 0;
    self->m_forward = true;
    self->m_moving = false;
    self->m_valid = false;
}

bool us_mtc_parser_parse(
    us_mtc_parser_t *self, const struct timeval *cur_time, const us_midi_msg_t *m, const us_midi_sysex_t *e, us_mtc_t *t) {
    /* check for MIDI quarter frame message */
    if (m != 0 && us_midi_msg_is_mtc(m)) {
        /* yup, got a quarter frame message. */
        return us_mtc_parser_parse_qf(self, cur_time, m->byte1, t);
    } else if (m == 0 || us_midi_msg_is_sysex(m)) /* check for MTC sysex */
        {
        /* make sure the sysex is a MTC sysex */
        const uint8_t *mb = us_buffer_contig_read_ptr(e->m_buffer);
        if (us_buffer_contig_readable_count(e->m_buffer) > 10) {
            if (mb[1] == 0x7f && mb[2] == 0x7f && mb[3] == 0x01 && mb[4] == 0x01) {
                /* yup! handle mtc sysex */
                return us_mtc_parser_parse_sysex(self, cur_time, e, t);
            }
        }
    }
    /* unrecognized message. return false */
    return false;
}

bool us_mtc_parser_poll(us_mtc_parser_t *self, const struct timeval *cur_time, us_mtc_t *t) {
    if (self->m_valid) {
        if (us_net_timeout_hit(cur_time, &self->m_timeout_timestamp)) {
            /* we lost sync! */
            self->m_valid = false;
            t->m_hour = 0;
            t->m_minute = 0;
            t->m_second = 0;
            t->m_frame = 0;
            t->m_fmt = US_MTC_RATE_UNKNOWN;
            return true; /* time code has changed! */
        }
    }
    return false;
}

bool us_mtc_parser_parse_qf(us_mtc_parser_t *self, const struct timeval *cur_time, uint8_t qf_value, us_mtc_t *t) {
    int type;
    uint8_t nibble_num = (qf_value >> 4) & 0x7;
    uint8_t nibble_val = (qf_value) & 0xf;
    /* update current nibble state */
    self->m_nibbles[nibble_num] = nibble_val;
    if (nibble_num == 0 && !self->m_seen_first) {
        self->m_valid = false;
        self->m_seen_first = true;
    }
    /* is this a nibble 7? */
    if (nibble_num == 7 && self->m_seen_first) {
        self->m_valid = true;
    }
    /* have we seen a nibble before ? */
    if (self->m_last_nibble != -1) {
        /* yes, check to see the direction that we are moving in */
        if ((self->m_last_nibble == 7 && nibble_num == 0) || (self->m_last_nibble + 1 == (int)nibble_num)) {
            /* we are going forward */
            self->m_forward = true;
            self->m_moving = true;
        } else if ((self->m_last_nibble == 0 && nibble_num == 7) || (self->m_last_nibble == (int)(nibble_num + 1))) {
            /* we are going backward */
            self->m_forward = false;
            self->m_moving = true;
        } else {
            /* we must have skipped a nibble - this means the timecode
             * must have changed significantly and we do not know
             * anything now
             */
            self->m_valid = false;
            self->m_seen_first = false;
            /* we dont know what the format is now */
            self->m_last_formed_time.m_fmt = US_MTC_RATE_UNKNOWN;
            /* we dont know which way we are moving, if at all */
            self->m_forward = false;
            self->m_moving = false;
            /*
             * if this nibble is nibble 0, then
             * this nibble is now the first official
             * start nibble that we have seen
             */
            if (nibble_num == 0) {
                self->m_first_nibble = nibble_num;
                self->m_seen_first = true;
            }
            /*
             * we have not seen the last nibble yet
             */
            self->m_last_nibble = -1;
        }
    }
    /* if the current time is valid now, then convert the nibbles to the time */
    self->m_qf = nibble_num;
    self->m_last_formed_time.m_frame = 0x1F & ((self->m_nibbles[1] << 4) + (self->m_nibbles[0] << 0));
    self->m_last_formed_time.m_second = 0x3F & ((self->m_nibbles[3] << 4) + (self->m_nibbles[2] << 0));
    self->m_last_formed_time.m_minute = 0x3F & ((self->m_nibbles[5] << 4) + (self->m_nibbles[4] << 0));
    self->m_last_formed_time.m_hour = 0x1F & ((self->m_nibbles[7] << 4) + (self->m_nibbles[6] << 0));
    type = (self->m_nibbles[7] >> 1) & 7;
    switch (type) {
    case 0:
        self->m_last_formed_time.m_fmt = US_MTC_RATE_24;
        break;
    case 1:
        self->m_last_formed_time.m_fmt = US_MTC_RATE_25;
        break;
    case 2:
        self->m_last_formed_time.m_fmt = US_MTC_RATE_30;
        break;
    case 3:
        self->m_last_formed_time.m_fmt = US_MTC_RATE_30;
        break;
    }
    /* the current nibble is now the last seen nibble */
    self->m_last_nibble = nibble_num;
    /* if we have already formed a time, copy the time over.
     */
    if (self->m_valid) {
        t->m_hour = self->m_last_formed_time.m_hour;
        t->m_minute = self->m_last_formed_time.m_minute;
        t->m_second = self->m_last_formed_time.m_second;
        t->m_frame = self->m_last_formed_time.m_frame;
        t->m_fmt = self->m_last_formed_time.m_fmt;
        us_net_timeout_add(&self->m_timeout_timestamp, cur_time, 1000000);
    } else {
        /* we dont have a valid time. Zero out the callers MTC struct */
        t->m_hour = 0;
        t->m_minute = 0;
        t->m_second = 0;
        t->m_frame = 0;
        t->m_fmt = US_MTC_RATE_UNKNOWN;
    }
    /* return true only if qf==0 and time is valid */
    if (self->m_moving && self->m_forward && self->m_qf == 7 && self->m_valid) {
        return true;
    }
    if (self->m_moving && !self->m_forward && self->m_qf == 0 && self->m_valid) {
        return true;
    }
    return false;
}

bool us_mtc_parser_parse_sysex(us_mtc_parser_t *self, const struct timeval *cur_time, const us_midi_sysex_t *e, us_mtc_t *t) {
    int type;
    /* read the fields of the sysex and store into self */
    const uint8_t *mb;
    size_t len;
    mb = us_buffer_contig_read_ptr(e->m_buffer);
    len = us_buffer_contig_readable_count(e->m_buffer);
    if (len > 10) {
        type = (mb[5] & 96) >> 5;
        self->m_last_formed_time.m_hour = mb[6];
        self->m_last_formed_time.m_minute = mb[7];
        self->m_last_formed_time.m_second = mb[8];
        self->m_last_formed_time.m_frame = mb[9];
        /* figure out how many frames for this type of code */
        switch (type) {
        case 0:
            self->m_last_formed_time.m_fmt = US_MTC_RATE_24;
            break;
        case 1:
            self->m_last_formed_time.m_fmt = US_MTC_RATE_25;
            break;
        case 2:
            self->m_last_formed_time.m_fmt = US_MTC_RATE_30;
            break;
        case 3:
            self->m_last_formed_time.m_fmt = US_MTC_RATE_30;
            break;
        default:
            self->m_last_formed_time.m_fmt = US_MTC_RATE_UNKNOWN;
            break;
        }
        /* we are not going forward or moving. But the
         ** time code is valid
         */
        self->m_valid = true;
        /* copy the entire time to *t */
        t->m_hour = self->m_last_formed_time.m_hour;
        t->m_minute = self->m_last_formed_time.m_minute;
        t->m_second = self->m_last_formed_time.m_second;
        t->m_frame = self->m_last_formed_time.m_frame;
        t->m_fmt = self->m_last_formed_time.m_fmt;
        /* nibbelize the time */
        self->m_nibbles[0] = (self->m_last_formed_time.m_frame >> 0) & 0xf;
        self->m_nibbles[1] = (self->m_last_formed_time.m_frame >> 4) & 0xf;
        self->m_nibbles[2] = (self->m_last_formed_time.m_second >> 0) & 0xf;
        self->m_nibbles[3] = (self->m_last_formed_time.m_second >> 4) & 0xf;
        self->m_nibbles[4] = (self->m_last_formed_time.m_minute >> 0) & 0xf;
        self->m_nibbles[5] = (self->m_last_formed_time.m_minute >> 4) & 0xf;
        self->m_nibbles[6] = (self->m_last_formed_time.m_hour >> 0) & 0xf;
        self->m_nibbles[7] = ((self->m_last_formed_time.m_hour >> 4) & 0x1) | ((type << 1) & 0xE);
        self->m_first_nibble = -1;
        self->m_last_nibble = -1;
        us_net_timeout_add(&self->m_timeout_timestamp, cur_time, 1000000);
    }
    return true;
}
