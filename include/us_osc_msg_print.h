#ifndef US_OSC_MSG_PRINT_H
#define US_OSC_MSG_PRINT_H

/*
Copyright (c) 2013, J.D. Koftinoff Software, Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of J.D. Koftinoff Software, Ltd. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD..  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"

#include "us_osc_msg.h"

#if US_ENABLE_PRINTING
#include "us_print.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 \addtogroup us_osc_msg OSC Message and Bundle Objects
 */
/*@{*/

/**
 Print an OSC Message Bundle object

 @param self ptr to bundle
 @param printer to print to
 @return bool true on success
 */
bool us_osc_msg_bundle_print( const us_osc_msg_bundle_t *self, us_print_t *printer );

/**
 Print an OSC Message Message object

 @param self ptr to msg
 @param printer to print to
 @return bool true on success
 */
bool us_osc_msg_print( const us_osc_msg_t *self, us_print_t *printer );

/**
 Print an OSC Message Message Element object

 @param self ptr to message element
 @param printer to print to
 @return bool true on success
 */
bool us_osc_msg_element_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_a_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_B_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_b_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_d_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_f_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_h_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_i_print( const us_osc_msg_element_t *self, us_print_t *printer );
bool us_osc_msg_element_s_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_t_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_T_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_F_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_M_print( const us_osc_msg_element_t *self, us_print_t *printer );

bool us_osc_msg_element_N_print( const us_osc_msg_element_t *self, us_print_t *printer );

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
#endif
