#ifndef US_MTC_PARSER_H
#define US_MTC_PARSER_H

#include "us_midi.h"
#include "us_mtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup tc_parser MIDI Time Code Parser
 */
/*@{*/

typedef struct us_mtc_parser_s
{
    uint8_t m_nibbles[8];

    int m_first_nibble;
    int m_last_nibble;
    bool m_seen_first;

    us_mtc_t m_last_formed_time;
    struct timeval m_timeout_timestamp;

    int m_qf;
    bool m_forward;
    bool m_moving;
    bool m_valid;
} us_mtc_parser_t;

void us_mtc_parser_init( us_mtc_parser_t *self );
bool us_mtc_parser_parse(
    us_mtc_parser_t *self, const struct timeval *cur_time, const us_midi_msg_t *m, const us_midi_sysex_t *e, us_mtc_t *t );

bool us_mtc_parser_poll( us_mtc_parser_t *self, const struct timeval *cur_time, us_mtc_t *t );

bool us_mtc_parser_parse_qf( us_mtc_parser_t *self, const struct timeval *cur_time, uint8_t qf_value, us_mtc_t *t );

bool us_mtc_parser_parse_sysex( us_mtc_parser_t *self, const struct timeval *cur_time, const us_midi_sysex_t *e, us_mtc_t *t );

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
