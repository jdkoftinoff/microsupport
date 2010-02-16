#include "us_world.h"

#ifdef US_CONFIG_WIN32

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif


int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= us_DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
  return 0;
}

bool us_platform_init_winsock( void )
{
  WSADATA wsaData;
  WORD version;
  int error;

  version = MAKEWORD( 2, 2 );

  error = WSAStartup( version, &wsaData );

  if ( error != 0 )
  {
    return false;
  }
  if ( version != wsaData.wVersion )
  {
    return false;
  }
  return true;
}

#endif


void us_gettimeofday( struct timeval *tv )
{
  int r;
  struct timezone tz;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;

  r = gettimeofday( tv, &tz );
  if( r!=0 )
  {
    perror( "gettimeofday" );
    abort();
  }
}


