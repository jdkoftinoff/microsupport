#ifndef US_SERIAL_H
#define US_SERIAL_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /** \addtogroup us_serial
  */
  /*@{*/

  int us_serial_open(
                     const char *serial_device,
                     const char *serial_options
                     );

  void us_serial_close( int fd );

  /*@}*/

#ifdef __cplusplus
}
#endif


#endif
