
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

#include "us_midi.h"

const signed char us_midi_lut_msglen[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    3, /**< 0x80=note off, 3 bytes */
    3, /**< 0x90=note on, 3 bytes */
    3, /**< 0xa0=poly pressure, 3 bytes */
    3, /**< 0xb0=control change, 3 bytes */
    2, /**< 0xc0=program change, 2 bytes */
    2, /**< 0xd0=channel pressure, 2 bytes */
    3, /**< 0xe0=pitch bend, 3 bytes */
    -1 /**< 0xf0=other things. may vary. */
};

const signed char us_midi_lut_sysmsglen[16] =
{
    -1,/**< 0xf0=sysex start. may vary */
    2, /**< 0xf1=MIDI Time Code. 2 bytes */
    3, /**< 0xf2=MIDI Song position. 3 bytes */
    2, /**< 0xf3=MIDI Song Select. 2 bytes. */
    0, /**< 0xf4=undefined */
    0, /**< 0xf5=undefined */
    1, /**< 0xf6=TUNE Request */
    0, /**< 0xf7=sysex end. */
    1, /**< 0xf8=timing clock. 1 byte */
    1, /**< 0xf9=proposed measure end? */
    1, /**< 0xfa=start. 1 byte */
    1, /**< 0xfb=continue. 1 byte */
    1, /**< 0xfc=stop. 1 byte */
    0, /**< 0xfd=undefined */
    1, /**< 0xfe=active sensing. 1 byte */
    3  /**< 0xff= not reset, but a META-EVENT, which is always 3 bytes */
};


const bool us_midi_lut_is_white[12] =
{
    /* C C#  D D#  E    F F# G G# A A# B */
    1, 0,  1, 0,  1,   1, 0, 1, 0, 1, 0, 1
};


const char * us_midi_chan_msg_name[16] =
{
    "ERROR 00    ",  /**< 0x00 */
    "ERROR 10    ",  /**< 0x10 */
    "ERROR 20    ",  /**< 0x20 */
    "ERROR 30    ",  /**< 0x30 */
    "ERROR 40    ",  /**< 0x40 */
    "ERROR 50    ",  /**< 0x50 */
    "ERROR 60    ",  /**< 0x60 */
    "ERROR 70    ",  /**< 0x70 */
    "NOTE OFF    ",  /**< 0x80 */
    "NOTE ON     ",  /**< 0x90 */
    "POLY PRES.  ",  /**< 0xa0 */
    "CTRL CHANGE ",  /**< 0xb0 */
    "PG CHANGE   ",  /**< 0xc0 */
    "CHAN PRES.  ",  /**< 0xd0 */
    "BENDER      ",  /**< 0xe0 */
    "SYSTEM      "   /**< 0xf0 */
};

const char *  us_midi_sys_msg_name[16] =
{
    "SYSEX       ",  /**< 0xf0 */
    "MTC         ",  /**< 0xf1 */
    "SONG POS    ",  /**< 0xf2 */
    "SONG SELECT ",  /**< 0xf3 */
    "ERR - F4    ",  /**< 0xf4 */
    "ERR - F5    ",  /**< 0xf5 */
    "TUNE REQ.   ",  /**< 0xf6 */
    "SYSEX END   ",  /**< 0xf7 */
    "CLOCK       ",  /**< 0xf8 */
    "MEASURE END ",  /**< 0xf9 */
    "START       ",  /**< 0xfa */
    "CONTINUE    ",  /**< 0xfb */
    "STOP        ",  /**< 0xfc */
    "ERR - FD    ",  /**< 0xfd */
    "SENSE       ",  /**< 0xfe */
    "META-EVENT  "   /**< 0xff */
};


const char * us_midi_msg_to_text ( const us_midi_msg_t *self, char *txt, int max_len )
{
    char buf[64];
    int buf_len = sizeof ( buf );
    int len = us_midi_msg_get_length ( self );
    if( max_len<64 )
    {
        return 0;
    }
    *txt = 0;
    if ( us_midi_msg_is_all_notes_off ( self ) )
    {
        US_DEFAULT_SNPRINTF ( buf, buf_len, "Ch %2d  All Notes Off  (ctrl=%3d)", ( int ) us_midi_msg_get_channel ( self ) + 1, ( int ) self->byte1 );
        strcat ( txt, buf );
    }
    else
    {
        int type = ( self->status & 0xf0 ) >> 4;
        /* if it is a note on with vel=0, call it a NOTE OFF */
        if ( type == 9 && self->byte2 == 0 )
            type = 8;
        if ( type != 0xf )
        {
            US_DEFAULT_SNPRINTF ( buf, buf_len, "Ch %2d  ", ( int ) us_midi_msg_get_channel ( self ) + 1 );
            strcat ( txt, buf );
        }
        strcat ( txt, us_midi_chan_msg_name[type] );
        if ( self->status >= 0xf0 )
        {
            strcat ( txt, us_midi_sys_msg_name[self->status-0xf0] );
            if ( len > 1 )
            {
                US_DEFAULT_SNPRINTF ( buf, buf_len, "%02x", ( int ) self->byte1 );
                strcat ( txt, buf );
            }
            if ( len > 2 )
            {
                US_DEFAULT_SNPRINTF ( buf, buf_len, ",%02x", ( int ) self->byte2 );
                strcat ( txt, buf );
            }
            if ( len > 3 )
            {
                US_DEFAULT_SNPRINTF ( buf, buf_len, ",%02x", ( int ) self->byte3 );
                strcat ( txt, buf );
            }
        }
        else
        {
            int32_t l = ( int32_t ) strlen ( txt );
            char *endtxt = txt + l;
            int32_t mx = 64 - l;
            switch ( self->status & 0xf0 )
            {
            case US_MIDI_NOTE_ON:
                if ( self->byte2 == 0 )
                    US_DEFAULT_SNPRINTF ( endtxt, mx, "Note %3d", ( int ) self->byte1 );
                else
                    US_DEFAULT_SNPRINTF ( endtxt, mx, "Note %3d  Vel  %3d  ", ( int ) self->byte1, ( int ) self->byte2 );
                break;
            case US_MIDI_NOTE_OFF:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "Note %3d  Vel  %3d  ", ( int ) self->byte1, ( int ) self->byte2 );
                break;
            case US_MIDI_POLY_PRESSURE:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "Note %3d  Pres %3d  ", ( int ) self->byte1, ( int ) self->byte2 );
                break;
            case US_MIDI_CONTROL_CHANGE:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "Ctrl %3d  Val  %3d  ", ( int ) self->byte1, ( int ) self->byte2 );
                break;
            case US_MIDI_PROGRAM_CHANGE:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "PG   %3d  ", ( int ) self->byte1 );
                break;
            case US_MIDI_CHANNEL_PRESSURE:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "Pres %3d  ", ( int ) self->byte1 );
                break;
            case US_MIDI_PITCH_BEND:
                US_DEFAULT_SNPRINTF ( endtxt, mx, "Val %5d", ( int ) us_midi_msg_get_bender_value ( self ) );
                break;
            }
        }
    }
    /* pad the rest with spaces */
    {
        int32_t l = ( int32_t ) strlen ( txt );
        char *p = txt + l;
        while ( l < 45 )
        {
            *p++ = ' ';
            ++l;
        }
        *p = '\0';
    }
    return txt;
}


int8_t us_midi_msg_get_length ( const us_midi_msg_t *self )
{
    if ( ( self->status & 0xf0 ) == 0xf0 )
    {
        return us_midi_get_system_message_length ( self->status );
    }
    else
    {
        return us_midi_get_message_length ( self->status );
    }
}

us_midi_sysex_t *us_midi_sysex_create ( us_allocator_t *allocator, int32_t max_length )
{
    us_midi_sysex_t *r = 0;
    us_midi_sysex_t *self = 0;
    self = us_new ( allocator, us_midi_sysex_t );
    if ( self )
    {
        self->destroy = us_midi_sysex_destroy;
        self->m_allocator = allocator;
        self->m_buffer = us_buffer_create ( allocator, max_length );
        if ( self->m_buffer )
        {
            r = self;
        }
        if ( r == 0 )
        {
            us_midi_sysex_destroy ( self );
        }
    }
    return r;
}

void us_midi_sysex_destroy ( us_midi_sysex_t *self )
{
    if ( self->m_buffer )
    {
        self->m_buffer->destroy ( self->m_buffer );
    }
    if ( self->m_allocator )
    {
        self->m_allocator->free ( self->m_allocator, self );
    }
}

us_midi_parser_t *us_midi_parser_create ( us_allocator_t *allocator, int32_t max_sysex_size )
{
    us_midi_parser_t *r = 0;
    us_midi_parser_t *self = 0;
    self = us_new ( allocator, us_midi_parser_t );
    if ( self )
    {
        self->destroy = us_midi_parser_destroy;
        self->m_allocator = allocator;
        us_midi_msg_init ( &self->m_tmp_msg );
        self->m_sysex = us_midi_sysex_create ( allocator, max_sysex_size );
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        if ( self->m_sysex )
        {
            r = self;
        }
        if ( r == 0 )
        {
            us_midi_parser_destroy ( self );
        }
    }
    return r;
}


void us_midi_parser_destroy ( us_midi_parser_t *self )
{
    if ( self->m_sysex )
    {
        self->m_sysex->destroy ( self->m_sysex );
    }
    if ( self->m_allocator )
    {
        self->m_allocator->free ( self->m_allocator, self );
    }
}

bool us_midi_parser_parse ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg )
{
    /*
      No matter what state we are currently in we must deal
      with bytes with the high bit set first.
    */
    us_midi_msg_init ( msg );
    if ( b & 0x80 )
    {
        /*
          check for system messages (>=0xf0)
        */
        uint8_t status = ( uint8_t ) ( b & 0xf0 );
        if ( status == 0xf0 )
        {
            /*
              System messages get parsed by
              us_parser_system_byte()
            */
            return us_midi_parser_system_byte ( self, b, msg );
        }
        else
        {
            /*
              Otherwise, this is a new status byte.
            */
            us_midi_parser_status_byte ( self, b, msg );
            return false;
        }
    }
    else
    {
        /*
          Try to parse the data byte
        */
        return us_midi_parser_data_byte ( self, b, msg );
    }
}

bool us_midi_parser_system_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg )
{
    switch ( b )
    {
    case US_MIDI_RESET:
    {
        /*
        // a reset byte always re-initializes our state
        // machine.
        */
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        return false;
    }
    case US_MIDI_SYSEX_START:
    {
        //
        // start receiving sys-ex data
        //
        self->m_state = US_MIDI_PARSER_STATE_SYSEX_DATA;
        //
        // Prepare sysex buffer.
        //
        us_midi_sysex_start ( self->m_sysex );
        return false;
    }
    case US_MIDI_SYSEX_END:
    {
        //
        // We are finished receiving a sysex message.
        //
        //
        // If we were not in SYSEX_DATA mode, this
        // EOX means nothing.
        //
        if ( self->m_state != US_MIDI_PARSER_STATE_SYSEX_DATA )
        {
            return false;
        }
        //
        // reset the state machine
        //
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        //
        // finish up sysex buffer
        //
        us_midi_sysex_end ( self->m_sysex );
        //
        // return a MIDIMessage with status=SYSEX_START
        // so calling program can know to look at
        // the sysex buffer with GetSystemExclusive().
        //
        us_midi_msg_set_status ( msg, US_MIDI_SYSEX_START );
        return true;
    }
    case US_MIDI_MTC:
    {
        //
        // Go into FIRST_OF_ONE_NORUN state.
        // this is required because MTC (F1) is not
        // allowed to be running status.
        //
        us_midi_msg_set_status ( &self->m_tmp_msg, US_MIDI_MTC );
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_ONE_NORUN;
        return false;
    }
    case US_MIDI_SONG_POSITION:
    {
        //
        // This is a two data byte message, so go into
        // FIRST_OF_TWO state.
        //
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_TWO;
        us_midi_msg_set_status ( &self->m_tmp_msg, US_MIDI_SONG_POSITION );
        return false;
    }
    case US_MIDI_SONG_SELECT:
    {
        //
        // This is a one data byte message, so go into
        // the FIRST_OF_ONE state.
        //
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_ONE;
        us_midi_msg_set_status ( &self->m_tmp_msg, US_MIDI_SONG_SELECT );
        return false;
    }
    //
    // the one byte system messages.
    // these messages may interrupt any other message,
    // and therefore do not affect the current state or
    // running status.
    //
    case US_MIDI_TUNE_REQUEST:
    case US_MIDI_TIMING_CLOCK:
    case US_MIDI_MEASURE_END:
    case US_MIDI_START:
    case US_MIDI_CONTINUE:
    case US_MIDI_STOP:
    case US_MIDI_ACTIVE_SENSE:
    {
        us_midi_msg_set_status ( msg, b );
        return true;
    }
    default:
    {
        //
        // any other byte must be ignored.
        // It is either a communicatin error or
        // a new type of MIDI message.
        // go into FIND_STATUS state to ignore
        // any possible data bytes for this unknown message
        //
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        return false;
    }
    }
}

bool us_midi_parser_status_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg )
{
    int8_t len = us_midi_get_message_length ( b );
    (void)msg;
    if ( len == 2 )
    {
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_ONE;
        us_midi_msg_set_status ( &self->m_tmp_msg, b );
    }
    else if ( len == 3 )
    {
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_TWO;
        us_midi_msg_set_status ( &self->m_tmp_msg, b );
    }
    else
    {
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        us_midi_msg_set_status ( &self->m_tmp_msg, 0 );
    }
    return false;
}

bool us_midi_parser_data_byte ( us_midi_parser_t *self, uint8_t b, us_midi_msg_t *msg )
{
    switch ( self->m_state )
    {
    case US_MIDI_PARSER_STATE_FIND_STATUS:
    {
        //
        // just eat data bytes until we get a status byte
        //
        return false;
    }
    case US_MIDI_PARSER_STATE_FIRST_OF_ONE:
    {
        //
        // this is the only data byte of a message.
        // form the message and return it.
        //
        us_midi_msg_set_byte1 ( &self->m_tmp_msg, b );
        *msg = self->m_tmp_msg;
        //
        // stay in this state for running status
        //
        return true;
    }
    case US_MIDI_PARSER_STATE_FIRST_OF_TWO:
    {
        //
        // this is the first byte of a two byte message.
        // read it in. go to SECOND_OF_TWO state. do not
        // return anything.
        //
        us_midi_msg_set_byte1 ( &self->m_tmp_msg, b );
        self->m_state = US_MIDI_PARSER_STATE_SECOND_OF_TWO;
        return false;
    }
    case US_MIDI_PARSER_STATE_SECOND_OF_TWO:
    {
        //
        // this is the second byte of a two byte message.
        // read it in. form the message, and return in.
        // go back to FIRST_OF_TWO state to allow
        // running status.
        //
        us_midi_msg_set_byte2 ( &self->m_tmp_msg, b );
        self->m_state = US_MIDI_PARSER_STATE_FIRST_OF_TWO;
        *msg = self->m_tmp_msg;
        return true;
    }
    case US_MIDI_PARSER_STATE_FIRST_OF_ONE_NORUN:
    {
        //
        // Single data byte system message, like MTC.
        // form the message, return it, and go to FIND_STATUS
        // state. Do not allow running status.
        //
        us_midi_msg_set_byte1 ( &self->m_tmp_msg, b );
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        *msg = self->m_tmp_msg;
        return true;
    }
    case US_MIDI_PARSER_STATE_SYSEX_DATA:
    {
        //
        // store the byte into the sysex buffer. Stay
        // in this state. Only a status byte can
        // change our state.
        //
        us_midi_sysex_put_byte ( self->m_sysex, b );
        return false;
    }
    default:
    {
        //
        // UNKNOWN STATE! go into FIND_STATUS state
        //
        self->m_state = US_MIDI_PARSER_STATE_FIND_STATUS;
        return false;
    }
    }
    return false;
}

