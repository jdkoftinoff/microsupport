#include "us_world.h"

#include "us_time.h"

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
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



void us_timeout_add( struct timeval *result, struct timeval *cur_time, uint32_t microseconds_to_add )
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

bool us_timeout_calc( struct timeval *result, struct timeval *cur_time, struct timeval *next_time )
{
  bool r=false;
  if( us_timeout_hit( cur_time, next_time ) )
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

bool us_timeout_hit( struct timeval *cur_time, struct timeval *next_time )
{
  bool r=false;

  if( cur_time->tv_sec>next_time->tv_sec ||
     (cur_time->tv_sec==next_time->tv_sec && cur_time->tv_usec>=next_time->tv_usec ) )
  {
    r=true;
  }
  return r;
}
