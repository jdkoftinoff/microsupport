#include "us_world.hpp"
#include "us_mtc_parser.hpp"


void us_mtc_parser_init ( us_mtc_parser_t *self )
{
    int i;
    for ( i = 0; i < 8; ++i )
    {
        self->m_nibbles[i] = 0;
    }
    us_mtc_init ( &self->m_last_formed_time );
    self->m_seen_first = false;
    self->m_first_nibble = -1;
    self->m_last_nibble = -1;
    self->m_valid = false;
    self->m_last_qf_microseconds = 0;
    self->m_moving = false;
    self->m_forward = false;
}


bool us_mtc_parser_handle_qf (
    us_mtc_parser_t *self,
    us_mtc_t *t,
    uint32_t time_stamp,
    int qf_value
)
{
    int type;
    uint8_t nibble_num = ( qf_value >> 4 ) & 0x7;
    uint8_t nibble_val = ( qf_value ) & 0xf;
    /* update current nibble state */
    self->m_nibbles[nibble_num] = nibble_val;
    self->m_last_qf_microseconds = time_stamp;
    if ( nibble_num == 0 && !self->m_seen_first )
    {
        self->m_valid = false;
        self->m_seen_first = true;
    }
    /* is this a nibble 7? */
    if ( nibble_num == 7 && self->m_seen_first )
    {
        self->m_valid = true;
    }
    /* have we seen a nibble before ? */
    if ( self->m_last_nibble != -1 )
    {
        /* yes, check to see the direction that we are moving in */
        if (
            ( self->m_last_nibble == 7 && nibble_num == 0 ) ||
            ( self->m_last_nibble + 1 == ( int ) nibble_num )
        )
        {
            /* we are going forward */
            self->m_forward = true;
            self->m_moving = true;
        }
        else if ( ( self->m_last_nibble == 0 && nibble_num == 7 ) ||
                  ( self->m_last_nibble == ( int ) ( nibble_num + 1 ) )
                )
        {
            /* we are going backward */
            self->m_forward = false;
            self->m_moving = true;
        }
        else
        {
            /* we must have skipped a nibble - this means the timecode
             * must have changed significantly and we do not know
             * anything now
             */
            self->m_valid = false;
            self->m_seen_first = false;
            /* we dont know which way we are moving, if at all */
            self->m_forward = false;
            self->m_moving = false;
            /*
             * if this nibble is nibble 0, then
             * this nibble is now the first official
             * start nibble that we have seen
             */
            if ( nibble_num == 0 )
            {
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
    self->m_last_nibble = nibble_num;
    self->m_last_formed_time.m_frame =
        0x1F & ( ( self->m_nibbles[1] << 4 ) + ( self->m_nibbles[0] << 0 ) );
    self->m_last_formed_time.m_second =
        0x3F & ( ( self->m_nibbles[3] << 4 ) + ( self->m_nibbles[2] << 0 ) );
    self->m_last_formed_time.m_minute =
        0x3F & ( ( self->m_nibbles[5] << 4 ) + ( self->m_nibbles[4] << 0 ) );
    self->m_last_formed_time.m_hour =
        0x1F & ( ( self->m_nibbles[7] << 4 ) + ( self->m_nibbles[6] << 0 ) );
    type = ( self->m_nibbles[7] >> 1 ) & 3;
    self->m_last_formed_time.m_fmt = us_mtc_fmt_from_midi[ type ];
    /* the current nibble is now the last seen nibble */
    self->m_last_nibble = nibble_num;
    /* if we have already formed a time, copy the time over.
    */
    self->m_valid = us_mtc_valid ( &self->m_last_formed_time );
    if ( t )
    {
        if ( self->m_valid )
        {
            *t = self->m_last_formed_time;
        }
        else
        {
            /* we dont have a valid time. Zero out the callers MTC struct */
            us_mtc_init ( t );
        }
    }
    /* return true only if qf==0 and time is valid */
    if ( self->m_moving
            && self->m_forward
            && self->m_last_nibble == 7
            && self->m_valid )
    {
        return true;
    }
    if ( self->m_moving
            && !self->m_forward
            && self->m_last_nibble == 0
            && self->m_valid
       )
    {
        return true;
    }
    return false;
}



bool us_mtc_parser_handle_full (
    us_mtc_parser_t *self,
    us_mtc_t *t,
    uint32_t time_stamp,
    int h, int m, int s, int f,
    int fmt
)
{
    self->m_last_qf_microseconds = time_stamp;
    self->m_last_formed_time.m_fmt = us_mtc_fmt_from_midi[fmt];
    self->m_last_formed_time.m_hour = h;
    self->m_last_formed_time.m_minute = m;
    self->m_last_formed_time.m_second = s;
    self->m_last_formed_time.m_frame = f;
    /* we are not going forward or moving. But the
    ** time code can be valid
    */
    self->m_valid = us_mtc_valid ( &self->m_last_formed_time );
    self->m_forward = false;
    self->m_moving = false;
    self->m_first_nibble = -1;
    self->m_last_nibble = -1;
    if ( self->m_valid )
    {
        /* copy the entire time to *t */
        if ( t )
        {
            *t = self->m_last_formed_time;
        }
        /* nibbelize the time */
        self->m_nibbles[0] = ( self->m_last_formed_time.m_frame >> 0 ) & 0xf;
        self->m_nibbles[1] = ( self->m_last_formed_time.m_frame >> 4 ) & 0xf;
        self->m_nibbles[2] = ( self->m_last_formed_time.m_second >> 0 ) & 0xf;
        self->m_nibbles[3] = ( self->m_last_formed_time.m_second >> 4 ) & 0xf;
        self->m_nibbles[4] = ( self->m_last_formed_time.m_minute >> 0 ) & 0xf;
        self->m_nibbles[5] = ( self->m_last_formed_time.m_minute >> 4 ) & 0xf;
        self->m_nibbles[6] = ( self->m_last_formed_time.m_hour >> 0 ) & 0xf;
        self->m_nibbles[7] = ( ( self->m_last_formed_time.m_hour >> 4 ) & 0x1 ) | ( ( fmt << 1 ) & 0xE );
    }
    return self->m_valid;
}



bool us_mtc_parser_handle_sysex (
    us_mtc_parser_t *self,
    us_mtc_t *t,
    uint32_t time_stamp,
    uint8_t *sysex,
    int sysex_len
)
{
    /* read the fields of the sysex and store into self */
    if ( sysex_len != 10 )
        return false;
    if ( sysex[0] != 0xf0 || sysex[9] != 0xf7 )
        return false;
    return us_mtc_parser_handle_full (
               self,
               t,
               time_stamp,
               sysex[5] & 96,
               sysex[6],
               sysex[7],
               sysex[8],
               sysex[5] & 0x1f
           );
    return true;
}



bool us_mtc_parser_poll (
    us_mtc_parser_t *self,
    uint32_t time_stamp
)
{
    if ( self->m_valid )
    {
        uint32_t usec_since_last_qf = time_stamp - self->m_last_qf_microseconds;
        if ( usec_since_last_qf > 500000 ) /* TODO: Make this variable instead of half a second */
        {
            /* we lost sync! */
            self->m_valid = false;
            self->m_moving = false;
            self->m_forward = false;
            return true;  /* time code has changed! */
        }
    }
    return false;
}



