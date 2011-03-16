#ifndef US_MIDI_H
#define US_MIDI_H

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

#ifndef US_WORLD_H
#include "us_world.h"
#endif

#include "us_allocator.h"
#include "us_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
       \defgroup us_midi MIDI protocol definitions
    */
    /*@{*/

    /**
      MIDI Status bytes
     */

    enum us_midi_status_e
    {
        US_MIDI_NOTE_OFF = 0x80,       /**< Note off message with velocity */
        US_MIDI_NOTE_ON  = 0x90,       /**< Note on message with velocity or Note off if velocity is 0 */
        US_MIDI_POLY_PRESSURE = 0xa0,  /**< Polyphonic key pressure/aftertouch with note and pressure */
        US_MIDI_CONTROL_CHANGE = 0xb0, /**< Control change message with controller number and 7 bit value */
        US_MIDI_PROGRAM_CHANGE = 0xc0, /**< Program change message with 7 bit program number */
        US_MIDI_CHANNEL_PRESSURE = 0xd0, /**< Channel pressure/aftertouch with pressure */
        US_MIDI_PITCH_BEND = 0xe0,     /**< Channel bender with 14 bit bender value */
        US_MIDI_SYSEX_START = 0xf0,    /**< Start of a MIDI System-Exclusive message */
        US_MIDI_MTC  = 0xf1,           /**< Start of a two byte MIDI Time Code message */
        US_MIDI_SONG_POSITION = 0xf2,  /**< Start of a three byte MIDI Song Position message */
        US_MIDI_SONG_SELECT = 0xf3,    /**< Start of a two byte MIDI Song Select message */
        US_MIDI_TUNE_REQUEST = 0xf6,   /**< Single byte tune request message */
        US_MIDI_SYSEX_END = 0xf7,      /**< End of a MIDI System-Exclusive message */
        US_MIDI_RESET  = 0xff,         /**< 0xff is a reset byte on the serial line. We never used as reset in a MIDIMessage object, */
        US_MIDI_TIMING_CLOCK = 0xf8,   /**< 24 timing clocks per beat */
        US_MIDI_MEASURE_END = 0xf9,    /**< Measure end byte */
        US_MIDI_START  = 0xfa,         /**< Sequence start message */
        US_MIDI_CONTINUE = 0xfb,       /**< Sequence continue message */
        US_MIDI_STOP  = 0xfc,          /**< Sequence stop message */
        US_MIDI_ACTIVE_SENSE = 0xfe,   /**< Active sense message */
        US_MIDI_META_EVENT = 0xff      /**< 0xff means a meta event in our internal processing. */
    };


    /**
      Controller Numbers
     */

    enum us_midi_controller_e
    {
        US_MIDI_C_LSB  = 0x20,         /**< add this to a non-switch controller to access the LSB. */

        US_MIDI_C_GM_BANK = 0x00,      /**< general midi bank select */
        US_MIDI_C_MODULATION = 0x01,   /**< modulation */
        US_MIDI_C_BREATH = 0x02,       /**< breath controller */
        US_MIDI_C_FOOT  = 0x04,        /**< foot controller */
        US_MIDI_C_PORTA_TIME = 0x05,   /**< portamento time */
        US_MIDI_C_DATA_ENTRY = 0x06,   /**< data entry value */
        US_MIDI_C_MAIN_VOLUME = 0x07,  /**< main volume control */
        US_MIDI_C_BALANCE = 0x08,      /**< balance control */
        US_MIDI_C_PAN  = 0x0a,         /**< panpot stereo control */
        US_MIDI_C_EXPRESSION = 0x0b,   /**< expression control */
        US_MIDI_C_GENERAL_1 = 0x10,    /**< general purpose controller 1 */
        US_MIDI_C_GENERAL_2 = 0x11,    /**< general purpose controller 2 */
        US_MIDI_C_GENERAL_3 = 0x12,    /**< general purpose controller 3 */
        US_MIDI_C_GENERAL_4 = 0x13,    /**< general purpose controller 4 */

        US_MIDI_C_DAMPER = 0x40,       /**< hold pedal (sustain) */
        US_MIDI_C_PORTA  = 0x41,       /**< portamento switch */
        US_MIDI_C_SOSTENUTO = 0x42,    /**< sostenuto switch */
        US_MIDI_C_SOFT_PEDAL = 0x43,   /**< soft pedal */
        US_MIDI_C_HOLD_2 = 0x45,       /**< hold pedal 2 */

        US_MIDI_C_GENERAL_5 = 0x50,    /**< general purpose controller 5 */
        US_MIDI_C_GENERAL_6 = 0x51,    /**< general purpose controller 6 */
        US_MIDI_C_GENERAL_7 = 0x52,    /**< general purpose controller 7 */
        US_MIDI_C_GENERAL_8 = 0x53,    /**< general purpose controller 8 */

        US_MIDI_C_EFFECT_DEPTH = 0x5b, /**< external effects depth */
        US_MIDI_C_TREMELO_DEPTH = 0x5c, /**< tremelo depth */
        US_MIDI_C_CHORUS_DEPTH = 0x5d, /**< chorus depth */
        US_MIDI_C_CELESTE_DEPTH = 0x5e, /**< celeste (detune) depth */
        US_MIDI_C_PHASER_DEPTH = 0x5f, /**< phaser effect depth */

        US_MIDI_C_DATA_INC = 0x60,     /**< increment data value */
        US_MIDI_C_DATA_DEC = 0x61,     /**< decrement data value */

        US_MIDI_C_NONRPN_LSB = 0x62,   /**< non registered parameter LSB */
        US_MIDI_C_NONRPN_MSB = 0x63,   /**< non registered parameter MSB */

        US_MIDI_C_RPN_LSB = 0x64,      /**< registered parameter LSB */
        US_MIDI_C_RPN_MSB = 0x65,      /**< registered parameter MSB */



        US_MIDI_C_RESET  = 0x79,       /**< reset all controllers */

        US_MIDI_C_LOCAL  = 0x79,       /**< local control on/off */
        US_MIDI_C_ALL_NOTES_OFF = 0x7a, /**< all notes off */
        US_MIDI_C_OMNI_OFF = 0x7c,     /**< omni off, all notes off */
        US_MIDI_C_OMNI_ON = 0x7d,      /**< omni on, all notes off */
        US_MIDI_C_MONO  = 0x7e,        /**< mono on, all notes off */
        US_MIDI_C_POLY  = 0x7f         /**< poly on, all notes off */
    };

    extern const int8_t us_midi_lut_msglen[16];
    extern const int8_t us_midi_lut_sysmsglen[16];
    extern const bool us_midi_lut_is_white[12];

    /** Message Length function. Not valid for Meta-events (0xff). Returns -1 if you need to call
        get_system_message_length() instead.
    */

    static inline int8_t us_midi_get_message_length ( uint8_t status )
    {
        return us_midi_lut_msglen[status>>4];
    }

    /** Message Length of system messages. Not valid for Meta-events (0xff). Returns -1 if this message
        length is unknown until parsing is complete.
    */

    static inline int8_t us_midi_get_system_message_length ( uint8_t status )
    {
        return us_midi_lut_sysmsglen[status-0xf0];
    }

    /** Piano key color white test */

    static inline bool us_midi_is_note_white ( uint8_t note )
    {
        return us_midi_lut_is_white[ note%12 ];
    }

    /** Piano key color black test */

    static inline bool us_midi_is_note_black ( uint8_t note )
    {
        return !us_midi_lut_is_white[ note%12 ];
    }

    /** Note # to standard octave conversion */

    static inline int us_midi_get_note_octave ( uint8_t note )
    {
        return ( note / 12 ) - 1;
    }

    /**
       \name us_midi_msg MIDI Message definitions
    */
    /*@{*/

    typedef struct us_midi_msg_s
    {
        uint8_t status;
        uint8_t byte1;
        uint8_t byte2;
        uint8_t byte3; /**< byte 3 is only used for meta-events and to round out the structure size to 32 bits */
    } us_midi_msg_t;

    extern const char * us_midi_chan_msg_name[16];  /**< Simple ascii text strings describing each channel message type (0x8X to 0xeX) */
    extern const char * us_midi_sys_msg_name[16];   /**< Simple ascii text strings describing each system message type (0xf0 to 0xff)  */


    static inline void us_midi_msg_init ( us_midi_msg_t *self )
    {
        self->status = 0;
        self->byte1 = 0;
        self->byte2 = 0;
        self->byte3 = 0;
    }

    static inline void us_midi_msg_copy ( us_midi_msg_t *self, const us_midi_msg_t * m )
    {
        self->status = m->status;
        self->byte1 = m->byte1;
        self->byte2 = m->byte2;
        self->byte3 = m->byte3;
    }

    const char * us_midi_msg_to_text ( const us_midi_msg_t *self, char *txt, int max_len );

    int8_t us_midi_msg_get_length ( const us_midi_msg_t *self );

    /** Get the status byte of the message. */
    static inline uint8_t us_midi_msg_get_status ( const us_midi_msg_t *self )
    {
        return ( uint8_t ) self->status;
    }

    /** If the message is a channel message, this method returns the MIDI channel that the message is on. */
    static inline uint8_t us_midi_msg_get_channel ( const us_midi_msg_t *self )
    {
        return ( uint8_t ) ( self->status & 0x0f );
    }

    /** If the message is a channel message, this method returns the relevant top 4 bits which describe what type of channel message it is. */
    static inline uint8_t us_midi_msg_get_type ( const us_midi_msg_t *self )
    {
        return ( uint8_t ) ( self->status & 0xf0 );
    }

    /** If the message is some sort of meta-message, then GetMetaType returns the type byte. */
    static inline uint8_t us_midi_msg_get_meta_type ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** Access to the raw byte1 of the message */
    static inline uint8_t us_midi_msg_get_byte1 ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** Access to the raw byte2 of the message */
    static inline uint8_t us_midi_msg_get_byte2 ( const us_midi_msg_t *self )
    {
        return self->byte2;
    }

    /** Access to the raw byte3 of the message */
    static inline uint8_t us_midi_msg_get_byte3 ( const us_midi_msg_t *self )
    {
        return self->byte3;
    }

    /** If the message is a note on, note off, or poly aftertouch message, GetNote() returns the note number */
    static inline uint8_t us_midi_msg_get_note ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** If the message is a note on, note off, or poly aftertouch message, GetVelocity() returns the velocity or pressure */
    static inline uint8_t us_midi_msg_get_velocity ( const us_midi_msg_t *self )
    {
        return self->byte2;
    }

    /** If the message is a channel pressure message, GetChannelPressure() returns the pressure value. */
    static inline uint8_t us_midi_msg_get_channel_pressure ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** If the message is a 7 bit program change value, GetPGValue() returns the program number. */
    static inline uint8_t us_midi_msg_get_pg_value ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** If the message is a control change message, GetController() returns the controller number. */
    static inline uint8_t us_midi_msg_get_controller ( const us_midi_msg_t *self )
    {
        return self->byte1;
    }

    /** If the message is a control change message, GetControllerValue() returns the 7 bit controller value. */
    static inline uint8_t us_midi_msg_get_controller_value ( const us_midi_msg_t *self )
    {
        return self->byte2;
    }


    /** Set all bits of the status byte */
    static inline void us_midi_msg_set_status ( us_midi_msg_t *self, uint8_t s )
    {
        self->status = s;
    }

    /** set just the lower 4 bits of the status byte without changing the upper 4 bits */
    static inline void us_midi_msg_set_channel ( us_midi_msg_t *self, uint8_t s )
    {
        self->status = ( uint8_t ) ( ( self->status & 0xf0 ) | s );
    }

    /** set just the upper 4 bits of the status byte without changing the lower 4 bits */
    static inline void us_midi_msg_set_type ( us_midi_msg_t *self, uint8_t s )
    {
        self->status = ( uint8_t ) ( ( self->status & 0x0f ) | s );
    }

    /** Set the value of the data byte 1 */
    static inline void us_midi_msg_set_byte1 ( us_midi_msg_t *self, uint8_t b )
    {
        self->byte1 = b;
    }

    /** Set the value of the data byte 2 */
    static inline void us_midi_msg_set_byte2 ( us_midi_msg_t *self, uint8_t b )
    {
        self->byte2 = b;
    }

    /** Set the value of the data byte 3 */
    static inline void us_midi_msg_set_byte3 ( us_midi_msg_t *self,  uint8_t b )
    {
        self->byte3 = b;
    }

    /** Set the note number for note on, note off, and polyphonic aftertouch messages */
    static inline void us_midi_msg_set_note ( us_midi_msg_t *self, uint8_t n )
    {
        self->byte1 = n;
    }

    /** Set the velocity of a note on or note off message */
    static inline void us_midi_msg_set_velocity ( us_midi_msg_t *self, uint8_t v )
    {
        self->byte2 = v;
    }

    /** Set the program number of a program change message */
    static inline void us_midi_msg_set_pg_value ( us_midi_msg_t *self, uint8_t v )
    {
        self->byte1 = v;
    }

    /** Set the controller number of a control change message */
    static inline void us_midi_msg_set_controller ( us_midi_msg_t *self, uint8_t c )
    {
        self->byte1 = c;
    }

    /** Set the 7 bit controller value of a control change message */
    static inline void us_midi_msg_set_controller_value ( us_midi_msg_t *self, uint8_t v )
    {
        self->byte2 = v;
    }


    static inline int16_t us_midi_msg_get_bender_value ( const us_midi_msg_t *self )
    {
        return ( int16_t ) ( ( ( self->byte2 << 7 ) | self->byte1 ) - 8192 );
    }


    static inline uint16_t us_midi_msg_get_meta_value ( const us_midi_msg_t *self )
    {
        return ( uint16_t ) ( ( self->byte3 << 8 ) | self->byte2 );
    }

    static inline bool us_midi_msg_is_channel_msg ( const us_midi_msg_t *self )
    {
        return ( self->status >= 0x80 ) && ( self->status < 0xf0 );
    }


    static inline bool us_midi_msg_is_note_on ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_NOTE_ON ) && self->byte2;
    }


    static inline bool us_midi_msg_is_note_off ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_NOTE_OFF ) ||
               ( ( ( self->status & 0xf0 ) == US_MIDI_NOTE_ON ) && self->byte2 == 0 );
    }


    static inline bool us_midi_msg_is_poly_pressure ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_POLY_PRESSURE );
    }


    static inline bool us_midi_msg_is_control_change ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_CONTROL_CHANGE );
    }


    static inline bool us_midi_msg_is_program_change ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_PROGRAM_CHANGE );
    }


    static inline bool us_midi_msg_is_channel_pressure ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_CHANNEL_PRESSURE );
    }


    static inline bool us_midi_msg_is_pitch_bend ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_PITCH_BEND );
    }


    static inline bool us_midi_msg_is_system_message ( const us_midi_msg_t *self )
    {
        return ( self->status & 0xf0 ) == 0xf0;
    }


    static inline bool us_midi_msg_is_sysex ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_SYSEX_START );
    }


    static inline int16_t us_midi_msg_get_sysex_num ( const us_midi_msg_t *self )
    {
        return ( int16_t ) ( ( self->byte3 << 8 ) | self->byte2 );
    }


    static inline bool us_midi_msg_is_mtc ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_MTC );
    }


    static inline bool us_midi_msg_is_song_position ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_SONG_POSITION );
    }


    static inline bool us_midi_msg_is_song_select ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_SONG_SELECT );
    }


    static inline bool  us_midi_msg_is_tune_request ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_TUNE_REQUEST );
    }


    static inline bool us_midi_msg_is_meta_event ( const us_midi_msg_t *self )
    {
        return ( self->status == US_MIDI_META_EVENT );
    }


    static inline bool us_midi_msg_is_all_notes_off ( const us_midi_msg_t *self )
    {
        return ( ( self->status & 0xf0 ) == US_MIDI_CONTROL_CHANGE )
               && ( self->byte1 >= US_MIDI_C_ALL_NOTES_OFF );
    }


    static inline void us_midi_msg_set_bender_value ( us_midi_msg_t *self, int16_t v )
    {
        int16_t x = ( int16_t ) ( v + 8192 );
        self->byte1 = ( uint8_t ) ( x & 0x7f );
        self->byte2 = ( uint8_t ) ( ( x >> 7 ) & 0x7f );
    }


    static inline void us_midi_msg_set_meta_type ( us_midi_msg_t *self, uint8_t t )
    {
        self->byte1 = t;
    }



    static inline void us_midi_msg_set_meta_value ( us_midi_msg_t *self, uint16_t v )
    {
        self->byte2 = ( uint8_t ) ( v & 0xff );
        self->byte3 = ( uint8_t ) ( ( v >> 8 ) & 0xff );
    }


    static inline void us_midi_msg_set_note_on ( us_midi_msg_t *self, uint8_t chan, uint8_t note, uint8_t vel )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_NOTE_ON );
        self->byte1 = note;
        self->byte2 = vel;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_note_off ( us_midi_msg_t *self, uint8_t chan, uint8_t note, uint8_t vel )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_NOTE_OFF );
        self->byte1 = note;
        self->byte2 = vel;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_poly_pressure ( us_midi_msg_t *self, uint8_t chan, uint8_t note, uint8_t pres )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_POLY_PRESSURE );
        self->byte1 = note;
        self->byte2 = pres;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_control_change ( us_midi_msg_t *self, uint8_t chan, uint8_t ctrl, uint8_t val )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_CONTROL_CHANGE );
        self->byte1 = ctrl;
        self->byte2 = val;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_program_change ( us_midi_msg_t *self, uint8_t chan, uint8_t val )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_PROGRAM_CHANGE );
        self->byte1 = val;
        self->byte2 = 0;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_channel_pressure ( us_midi_msg_t *self, uint8_t chan, uint8_t val )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_CHANNEL_PRESSURE );
        self->byte1 = val;
        self->byte2 = 0;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_pitch_bend ( us_midi_msg_t *self, uint8_t chan, int16_t val )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_PITCH_BEND );
        val += ( int16_t ) 0x2000; /* center value */
        self->byte1 = ( uint8_t ) ( val & 0x7f ); /* 7 bit bytes */
        self->byte2 = ( uint8_t ) ( ( val >> 7 ) & 0x7f );
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_pitch_bend_bytes ( us_midi_msg_t *self, uint8_t chan, uint8_t low, uint8_t high )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_PITCH_BEND );
        self->byte1 = ( uint8_t ) ( low );
        self->byte2 = ( uint8_t ) ( high );
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_sysex ( us_midi_msg_t *self, uint16_t num )
    {
        self->status = US_MIDI_SYSEX_START;
        self->byte1 = 0;
        self->byte2 = ( uint8_t ) ( num & 0xff );
        self->byte3 = ( uint8_t ) ( ( num >> 8 ) & 0xff );
    }


    static inline void us_midi_msg_set_mtc ( us_midi_msg_t *self, uint8_t field, uint8_t v )
    {
        self->status = US_MIDI_MTC;
        self->byte1 = ( uint8_t ) ( ( field << 4 ) | v );
        self->byte2 = 0;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_song_position ( us_midi_msg_t *self, int16_t pos )
    {
        self->status = US_MIDI_SONG_POSITION;
        self->byte1 = ( uint8_t ) ( pos & 0x7f );
        self->byte2 = ( uint8_t ) ( ( pos >> 7 ) & 0x7f );
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_song_select ( us_midi_msg_t *self, uint8_t sng )
    {
        self->status = US_MIDI_SONG_SELECT;
        self->byte1 = sng;
        self->byte2 = 0;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_tune_request ( us_midi_msg_t *self )
    {
        self->status = US_MIDI_TUNE_REQUEST;
        self->byte1 = 0;
        self->byte2 = 0;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_meta_event_bytes ( us_midi_msg_t *self, uint8_t type, uint8_t v1, uint8_t v2 )
    {
        self->status = US_MIDI_META_EVENT;
        self->byte1 = type;
        self->byte2 = v1;
        self->byte3 = v2;
    }


    static inline void us_midi_msg_set_meta_event ( us_midi_msg_t *self, uint8_t type, uint16_t v )
    {
        self->status = US_MIDI_META_EVENT;
        self->byte1 = type;
        self->byte2 = ( uint8_t ) ( v & 0xff );
        self->byte3 = ( uint8_t ) ( ( v >> 8 ) & 0xff );
    }


    static inline void us_midi_msg_set_all_notes_off (
        us_midi_msg_t *self,
        uint8_t chan,
        uint8_t type
    )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_CONTROL_CHANGE );
        self->byte1 = type;
        self->byte2 = 0x7f;
        self->byte3 = 0;
    }


    static inline void us_midi_msg_set_local ( us_midi_msg_t *self, uint8_t chan, uint8_t v )
    {
        self->status = ( uint8_t ) ( chan | US_MIDI_CONTROL_CHANGE );
        self->byte1 = US_MIDI_C_LOCAL;
        self->byte2 = v;
        self->byte3 = 0;
    }

    /*@}*/

    /**
       \name us_midi_sysex MIDI SysEx Message Definitions
    */
    /*@{*/

    typedef struct us_midi_sysex_s
    {
        void ( *destroy ) ( struct us_midi_sysex_s *self );
        us_allocator_t *m_allocator;
        us_buffer_t *m_buffer;
    } us_midi_sysex_t;

    us_midi_sysex_t *us_midi_sysex_create ( us_allocator_t *allocator, int32_t max_length );
    void us_midi_sysex_destroy ( us_midi_sysex_t *self );

    static inline void us_midi_sysex_clear ( us_midi_sysex_t *self )
    {
        us_buffer_reset ( self->m_buffer );
    }

    static inline bool us_midi_sysex_put_byte ( us_midi_sysex_t *self, uint8_t b )
    {
        return us_buffer_append_byte ( self->m_buffer, ( int ) b );
    }

    static inline void us_midi_sysex_start ( us_midi_sysex_t *self )
    {
        us_midi_sysex_clear ( self );
        us_midi_sysex_put_byte ( self, US_MIDI_SYSEX_START );
    }

    static inline void us_midi_sysex_end ( us_midi_sysex_t *self )
    {
        us_midi_sysex_put_byte ( self, US_MIDI_SYSEX_END );
    }

    /** Low nibble first */
    static inline void us_midi_sysex_put_nibblized_byte_lh ( us_midi_sysex_t *self, uint8_t b )
    {
        us_midi_sysex_put_byte ( self, ( uint8_t ) ( b & 0xf ) );
        us_midi_sysex_put_byte ( self, ( uint8_t ) ( b >> 4 ) );
    }

    /** High nibble first */
    static inline void us_midi_sysex_put_nibblized_byte_hl ( us_midi_sysex_t *self, uint8_t b )
    {
        us_midi_sysex_put_byte ( self, ( uint8_t ) ( b >> 4 ) );
        us_midi_sysex_put_byte ( self, ( uint8_t ) ( b & 0xf ) );
    }

    static inline int32_t us_midi_sysex_get_length ( const us_midi_sysex_t *self )
    {
        return us_buffer_readable_count( self->m_buffer );
    }

    static inline uint8_t us_midi_sysex_get_data ( const us_midi_sysex_t *self, int i )
    {
        return us_buffer_peek( self->m_buffer, i);
    }

    static inline bool us_midi_sysex_is_full ( const us_midi_sysex_t *self )
    {
        return us_buffer_writable_count( self->m_buffer )==0;
    }

    /*@}*/

    /**
       \name us_midi_parser MIDI Parser Definitions
    */
    /*@{*/

    /** MIDI Parser State */
    typedef enum
    {
        US_MIDI_PARSER_STATE_FIND_STATUS,          /**< ignore data bytes */
        US_MIDI_PARSER_STATE_FIRST_OF_ONE,         /**< read first data byte of a one data byte msg  */
        US_MIDI_PARSER_STATE_FIRST_OF_TWO,         /**< read first data byte of a two data byte msg  */
        US_MIDI_PARSER_STATE_SECOND_OF_TWO,        /**< read second data byte of a two data byte msg */
        US_MIDI_PARSER_STATE_FIRST_OF_ONE_NORUN,   /**< read one byte message, do not allow running status (for MTC)    */
        US_MIDI_PARSER_STATE_SYSEX_DATA            /**< read sysex data byte */
    } us_midi_parser_state_t;

    typedef struct us_midi_parser_s
    {
        void ( *destroy ) ( struct us_midi_parser_s *self );
        us_allocator_t *m_allocator;
        us_midi_msg_t m_tmp_msg;
        us_midi_sysex_t *m_sysex;
        us_midi_parser_state_t m_state;
    } us_midi_parser_t;

    us_midi_parser_t *us_midi_parser_create ( us_allocator_t *allocator, int32_t max_sysex_size );
    void us_midi_parser_destroy ( us_midi_parser_t *self );
    bool us_midi_parser_parse ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg );
    bool us_midi_parser_status_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg );
    bool us_midi_parser_system_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg );
    bool us_midi_parser_data_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg );
    static inline us_midi_sysex_t *us_midi_parser_get_sysex ( us_midi_parser_t *self )
    {
        return self->m_sysex;
    }
    /*@}*/

    /*@}*/

#ifdef __cplusplus
}
#endif

#endif
