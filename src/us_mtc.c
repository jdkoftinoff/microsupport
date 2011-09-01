#include "us_world.h"
#include "us_mtc.h"
#include "us_midi.h"

/**! Map of  rate codes to max frame counts */
const uint8_t us_mtc_max_frame[] =
{
    24,  /* tc_RATE_24 = 24 frames     */
    25,  /* tc_RATE_25 = 25 frames     */
    30,  /* tc_RATE_2997 = 30 frames   */
    30,  /* tc_RATE_2997DF = 30 frames */
    30,  /* tc_RATE_30 = 30 frames     */
    30,  /* tc_RATE_30DF = 30 frames   */
    30   /* unknown defaults to 30     */
};

/** Map of rate codes to drop frame flags */

const bool us_mtc_is_drop[] =
{
    false, /* tc_RATE_24 = ND       */
    false, /* tc_RATE_25 = ND       */
    false, /* tc_RATE_2997 = ND     */
    true,  /* tc_RATE_2997DF = DROP */
    false, /* tc_RATE_30 = ND       */
    true,  /* tc_RATE_30DF = DROP   */
    false  /* unknown defaults to ND*/
};

/*! Map of rate codes to MIDI type codes */
const uint8_t us_mtc_typecode[] =
{
    0, /* tc_RATE_24 */
    1, /* tc_RATE_25 */
    3, /* tc_RATE_2997 */
    2, /* tc_RATE_2997DF */
    3, /* tc_RATE_30 */
    2, /* tc_RATE_30DF */
    3, /* unknown defaults to 30 */
};

/*! Map of MIDI type codes to rate codes */
const uint8_t us_mtc_fmt_from_midi[] =
{
    US_MTC_RATE_24,
    US_MTC_RATE_25,
    US_MTC_RATE_2997DF, /* Note this is really 2997 DF not 30DF */
    US_MTC_RATE_30
};

/*! Map of rate codes to string descriptions */
const char *us_mtc_format_string[] =
{
    "24fps",
    "25fps",
    "29.97fps ND",
    "29.97fps D",
    "30fps ND",
    "30fps D",
    "23.98fps ND",
    "23.98fps F", 
    "unknown"
};

void us_mtc_init ( us_mtc_t *self )
{
    self->m_fmt = US_MTC_RATE_30;
    self->m_frame = 0;
    self->m_second = 0;
    self->m_minute = 0;
    self->m_hour = 0;
}

bool us_mtc_valid ( us_mtc_t *self )
{
    uint8_t lim = us_mtc_max_frame[ self->m_fmt ];

    if ( self->m_frame >= lim )
        return false;

    if ( self->m_second >= 60 )
        return false;

    if ( self->m_minute >= 60 )
        return false;

    if ( self->m_hour >= 24 )
        return false;

    if ( us_mtc_is_drop[ self->m_fmt ] )
    {
        bool min_is_divisible_by_ten = false;

        if ( ( self->m_minute % 10 ) == 0 )
            min_is_divisible_by_ten = true;

        if ( self->m_frame < 2 && self->m_second == 0 && !min_is_divisible_by_ten )
        {
            return false;
        }
    }

    return true;
}

void us_mtc_increment ( us_mtc_t *self )
{
    uint8_t lim = us_mtc_max_frame[ self->m_fmt ];

    if ( self->m_frame < lim - 1 )
    {
        self->m_frame++;
    }
    else
    {
        self->m_frame = 0;

        if ( self->m_second < 60 - 1 )
        {
            self->m_second ++;
        }
        else
        {
            self->m_second = 0;

            if ( self->m_minute < 60 - 1 )
            {
                self->m_minute++;
            }
            else
            {
                self->m_minute = 0;

                if ( self->m_hour < 24 - 1 )
                {
                    self->m_hour++;
                }
                else
                {
                    self->m_hour = 0;
                }
            }
        }
    }

    if ( us_mtc_is_drop[ self->m_fmt ] )
    {
        bool min_is_divisible_by_ten = false;

        if ( ( self->m_minute % 10 ) == 0 )
            min_is_divisible_by_ten = true;

        if ( self->m_frame == 0 && self->m_second == 0 && !min_is_divisible_by_ten )
        {
            self->m_frame = 2;
        }
    }
}

void us_mtc_decrement ( us_mtc_t *self )
{
    uint8_t lim = us_mtc_max_frame[ self->m_fmt ];

    if ( us_mtc_is_drop[ self->m_fmt ] )
    {
        bool min_is_divisible_by_ten = false;

        if ( ( self->m_minute % 10 ) == 0 )
            min_is_divisible_by_ten = true;

        if ( self->m_frame == 2 && self->m_second == 0 && !min_is_divisible_by_ten )
        {
            self->m_frame = 0;
        }
    }

    if ( self->m_frame > 0 )
    {
        self->m_frame--;
    }
    else
    {
        self->m_frame = lim - 1 ;

        if ( self->m_second > 0 )
        {
            self->m_second --;
        }
        else
        {
            self->m_second = 60 - 1;

            if ( self->m_minute > 0 )
            {
                self->m_minute--;
            }
            else
            {
                self->m_minute = 60 - 1;

                if ( self->m_hour > 0 )
                {
                    self->m_hour--;
                }
                else
                {
                    self->m_hour = 24 - 1;
                }
            }
        }
    }
}

bool us_mtc_print ( us_mtc_t *self, bool print_fmt, char *buf, size_t buf_len )
{
    bool r = true;
    int cnt;
    const char *fmt_spacing = "";
    const char *fmt = "";
    char df_indicator = ':';

    if ( us_mtc_is_drop[ self->m_fmt ] )
        df_indicator = ';';

    if ( print_fmt )
    {
        fmt = us_mtc_format_string[ self->m_fmt ];
        fmt_spacing = " ";
    }

#ifdef US_CONFIG_WIN32
    cnt = _snprintf_s ( buf, buf_len, _TRUNCATE, "%02d:%02d:%02d%c%02d%s%s",
                        ( int ) self->m_hour,
                        ( int ) self->m_minute,
                        ( int ) self->m_second,
                        ( int ) df_indicator,
                        ( int ) self->m_frame,
                        fmt_spacing,
                        fmt
                      );
#else
    cnt = snprintf ( buf, buf_len, "%02d:%02d:%02d%c%02d%s%s",
                     ( int ) self->m_hour,
                     ( int ) self->m_minute,
                     ( int ) self->m_second,
                     ( int ) df_indicator,
                     ( int ) self->m_frame,
                     fmt_spacing,
                     fmt
                   );
#endif

    if ( ( size_t ) cnt >= buf_len || cnt < 0 )
        r = false;

    return r;
}


bool us_mtc_parse (
    us_mtc_t *self,
    const char *ascii,
    us_mtc_format_t fmt
)
{
    bool r = false;
    int h = 0, m = 0, s = 0, f = 0;
    bool is_drop = false;
    int cnt;
    cnt = sscanf ( ascii, "%2d:%2d:%2d:%2d", &h, &m, &s, &f );

    if ( cnt == 4 )
    {
        is_drop = false;
    }
    else
    {
        cnt = sscanf ( ascii, "%2d:%2d:%2d;%2d", &h, &m, &s, &f );

        if ( cnt == 4 )
        {
            is_drop = true;
        }
    }

    if ( cnt == 4 )
    {
        self->m_hour = h;
        self->m_minute = m;
        self->m_second = s;
        self->m_frame = f;
        self->m_fmt = fmt;

        if ( us_mtc_is_drop[fmt] == is_drop )
        {
            r = us_mtc_valid ( self );
        }
    }

    return r;
}

int us_mtc_compare ( us_mtc_t *left, us_mtc_t *right )
{
    int32_t tf_left = us_mtc_get_total_frames ( left );
    int32_t tf_right = us_mtc_get_total_frames ( right );
    return tf_left - tf_right;
}

int32_t us_mtc_get_total_frames ( us_mtc_t *self )
{
    uint8_t fps = us_mtc_max_frame[ self->m_fmt ];
    int32_t total_frames = 0;
    /* calculate total frames */
    total_frames = ( ( ( self->m_hour * 3600 ) +
                       ( self->m_minute * 60 ) +
                       self->m_second )
                     * fps ) + self->m_frame;

    /* take into account dropped frames */
    if ( us_mtc_is_drop[ self->m_fmt ] )
    {
        /*  frame 0 and 1 omitted from first second of each minute, but included when minutes divides by ten */
        total_frames -= 108 * self->m_hour; /* 108 dropped frames per hour */
        total_frames -= 2 * self->m_minute; /* 2 dropped frames per minute */
        total_frames += 2 * ( self->m_minute / 10 ); /* add the 2 back in when minutes / 10 */
    }

    return total_frames;
}

void us_mtc_set_total_frames ( us_mtc_t *self, int32_t total_frames )
{
    uint8_t fps = us_mtc_max_frame[ self->m_fmt ];

    if ( us_mtc_is_drop[ self->m_fmt ] )
    {
        int32_t etf =  total_frames - ( 2 + ( ( total_frames / 17980 ) * 2 ) );
        total_frames += ( 2 * ( etf / 1798 ) ) - ( 2 * ( etf / 17980 ) );
    }

    self->m_frame = total_frames % fps;
    self->m_second = ( total_frames / fps ) % 60;
    self->m_minute = ( total_frames / ( fps * 60 ) ) % 60;
    self->m_hour = ( total_frames / ( fps * 3600 ) ) % 24;
}

int us_mtc_extract_qf ( us_mtc_t *self, int qf )
{
    int val;

    switch ( qf )
    {
    case 0:
        val = self->m_frame & 0xf;
        break;
    case 1:
        val = ( self->m_frame >> 4 ) & 0xf;
        break;
    case 2:
        val = self->m_second & 0xf;
        break;
    case 3:
        val = ( self->m_second >> 4 ) & 0xf;
        break;
    case 4:
        val = self->m_minute & 0xf;
        break;
    case 5:
        val = ( self->m_minute >> 4 ) & 0xf;
        break;
    case 6:
        val = self->m_hour & 0xf;
        break;
    case 7:
        val = ( ( self->m_hour >> 4 ) & 0x1 ) +
              ( us_mtc_typecode[ self->m_fmt ] << 1 );
        break;
    default:
        val = 0;
        break;
    }

    return val;
}


bool us_mtc_store_qf ( us_mtc_t *self, int qf, int qf_value )
{
    bool r = true;

    switch ( qf )
    {
    case 0:
        self->m_frame = ( self->m_frame & 0xf0 ) | ( ( qf_value & 0xf ) << 0 );
        break;
    case 1:
        self->m_frame = ( self->m_frame & 0x0f ) | ( ( qf_value & 0xf ) << 4 );
        break;
    case 2:
        self->m_second = ( self->m_second & 0xf0 ) | ( ( qf_value & 0xf ) << 0 );
        break;
    case 3:
        self->m_second = ( self->m_second & 0x0f ) | ( ( qf_value & 0xf ) << 4 );
        break;
    case 4:
        self->m_minute = ( self->m_minute & 0xf0 ) | ( ( qf_value & 0xf ) << 0 );
        break;
    case 5:
        self->m_minute = ( self->m_minute & 0x0f ) | ( ( qf_value & 0xf ) << 4 );
        break;
    case 6:
        self->m_hour = ( self->m_hour & 0xf0 ) | ( ( qf_value & 0xf ) << 0 );
        break;
    case 7:
        self->m_hour = ( self->m_hour & 0x0f ) | ( ( qf_value & 0x1 ) << 4 );
        self->m_fmt = us_mtc_fmt_from_midi[ ( qf_value & 0x6 ) >> 1 ];
        break;
    default:
        r = false;
        break;
    }

    return r;
}

