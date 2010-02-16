#ifndef US_NET_H
#define US_NET_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /** \addtogroup us_net
   */
  /*@{*/

  struct addrinfo *
  us_net_get_addrinfo(
                      const char *ip_addr,
                      const char *ip_port,
                      int type
                      );

  int us_net_create_udp_socket(
                               struct addrinfo *ai,
                               bool do_bind
                               );

  int us_net_create_tcp_socket(
                               struct addrinfo *ai,
                               bool do_bind
                               );

  void us_net_timout_add( struct timeval *result, struct timeval *cur_time, uint32_t microseconds_to_add );
  bool us_net_timeout_calc( struct timeval *result, struct timeval *cur_time, struct timeval *next_time );
  bool us_net_timeout_hit( struct timeval *cur_time, struct timeval *next_time );

  /*@}*/

#ifdef __cplusplus
}
#endif


#endif
