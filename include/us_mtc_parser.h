#ifndef US_MTC_PARSER_H
#define US_MTC_PARSER_H

#include "us_midi.h"
#include "us_mtc.h"

#ifdef __cplusplus
extern "C"
{
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
        uint32_t m_last_qf_microseconds;
        bool m_valid;
        bool m_moving;
        bool m_forward;
    } us_mtc_parser_t;

    void us_mtc_parser_init ( us_mtc_parser_t *self );

    bool us_mtc_parser_handle_qf (
        us_mtc_parser_t *self,
        us_mtc_t *t,
        uint32_t time_stamp,
        int qf_value
    );

    bool us_mtc_parser_handle_full (
        us_mtc_parser_t *self,
        us_mtc_t *t,
        uint32_t time_stamp,
        int h, int m, int s, int f,
        int fmt
    );

    bool us_mtc_parser_handle_sysex (
        us_mtc_parser_t *self,
        us_mtc_t *t,
        uint32_t time_stamp,
        uint8_t *sysex,
        int sysex_len
    );

    bool us_mtc_parser_poll (
        us_mtc_parser_t *self,
        uint32_t time_stamp
    );

    /*@}*/

#ifdef __cplusplus
}
#endif


#endif
