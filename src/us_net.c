#include "us_world.h"
#include "us_net.h"

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

#if US_ENABLE_BSD_SOCKETS
struct addrinfo *us_net_get_addrinfo(
                                        const char *ip_addr,
                                        const char *ip_port,
                                        int type
                                        )
{
  int e;
  struct addrinfo *ai;
  struct addrinfo hints;
  memset(&hints,'\0',sizeof(hints) );
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hints.ai_socktype = type;

  do
  {
    e = getaddrinfo( ip_addr,ip_port,&hints,&ai );
  } while( e==EAI_AGAIN );

  if( e!=0 )
  {
    fprintf( stderr, "getaddrinfo: %s\n", gai_strerror(e) );
    ai=0;
  }

  return ai;
}


int us_net_create_udp_socket(
                                struct addrinfo *ai,
                                bool do_bind
                                )
{
  int r = -1;
  int s = -1;

  if( ai )
  {
    s = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
    if( s>=0 )
    {
      int on =1;
      r=s;

      if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof(on)) == -1)
      {
        perror("setsockopt SO_BROADCAST:" );
        abort();
      }


      if( do_bind )
      {
        if( bind( s, ai->ai_addr, ai->ai_addrlen )==0 )
        {
          if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1)
          {
            perror("setsockopt SO_REUSEADDR:" );
            abort();
          }

          r=s;
        }
        else
        {
          perror( "socket: " );
        }
      }
    }
    else
    {
      perror( "socket: " );
    }

    return r;
  }

  if( r==-1 && s!=-1 )
  {
    closesocket(s);
  }

  return s;
}


int us_net_create_tcp_socket(
                                struct addrinfo *ai,
                                bool do_bind
                                )
{
  int r = -1;
  int s = -1;

  if( ai )
  {
    s = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
    if( s>=0 )
    {
      r=s;

      if( do_bind )
      {
        if( bind( s, ai->ai_addr, ai->ai_addrlen )==0 )
        {
          r=s;
        }
        else
        {
          perror( "socket: " );
        }
      }
    }
    else
    {
      perror( "socket: " );
    }

    return r;
  }

  if( r==-1 && s!=-1 )
  {
    closesocket(s);
  }

  return s;
}



void  us_net_timeout_add( struct timeval *result, struct timeval *cur_time, uint32_t microseconds_to_add )
{
  int32_t secs = microseconds_to_add/1000000;
  int32_t micros = microseconds_to_add%1000000;

  result->tv_sec = cur_time->tv_sec + secs;
  result->tv_usec = cur_time->tv_usec + micros;
  if( result->tv_usec>=1000000 )
  {
    result->tv_usec-=1000000;
    result->tv_sec++;
  }
}

bool  us_net_timeout_calc( struct timeval *result, struct timeval *cur_time, struct timeval *next_time )
{
  bool r=false;
  if( us_net_timeout_hit( cur_time, next_time ) )
  {
    /* timeout was hit already, so hack it to 1 sec */
    result->tv_sec=1;
    result->tv_usec=0;
  }
  else
  {
    result->tv_sec = next_time->tv_sec - cur_time->tv_sec;
    result->tv_usec = next_time->tv_usec - cur_time->tv_usec;
    if( result->tv_usec <0 )
    {
      result->tv_usec+=1000000;
      result->tv_sec--;
    }
    r=true;
  }
  return r;
}

bool us_net_timeout_hit( struct timeval *cur_time, struct timeval *next_time )
{
  bool r=false;

  if( cur_time->tv_sec>next_time->tv_sec ||
     (cur_time->tv_sec==next_time->tv_sec && cur_time->tv_usec>=next_time->tv_usec ) )
  {
    r=true;
  }
  return r;
}

#endif

