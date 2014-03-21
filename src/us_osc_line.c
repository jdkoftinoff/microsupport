#include "us_world.h"

#include "us_osc_line.h"
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

#if 0
bool us_osc_line_parse(
    us_allocator_t *allocator, const char *line, size_t line_len, us_osc_msg_t **osc_msg, us_osc_msg_bundle_t **osc_bundle) {
	// TODO:

    return false;
}

bool us_osc_line_parse_msg(us_allocator_t *allocator, const char *line, size_t line_len, us_osc_msg_t **osc_msg) {
    return false;
}

bool us_osc_line_parse_bundle(us_allocator_t *allocator, const char *line, size_t line_len, us_osc_msg_bundle_t **osc_bundle) {
    return false;
}
#endif

bool us_osc_line_gen_msg(const us_osc_msg_t *msg, us_print_t *result) {
    bool r = true;
    const us_osc_msg_element_t *element = msg->m_first_element;
    r &= result->printf(result, "%s", msg->m_address);
    while (element) {
        r &= result->printf(result, " %c", element->m_code);
        switch (element->m_code) {
        case 'a': {
            const us_osc_msg_element_a_t *e = (const us_osc_msg_element_a_t *)element;
            r &= result->printf(result, " %08lx%08lx", e->m_time_high, e->m_time_low);
            break;
        }
        case 'B': {
            const us_osc_msg_element_B_t *e = (const us_osc_msg_element_B_t *)element;
            r &= result->printf(result, " ");
            r &= us_osc_line_gen_bundle(e->m_bundle, result);
            break;
        }
        case 'b': {
            const us_osc_msg_element_b_t *e = (const us_osc_msg_element_b_t *)element;
            int i;
            for (i = 0; i < e->m_length; ++i) {
                r &= result->printf(result, " %02x", e->m_data[i]);
            }
            break;
        }
        case 'd': {
            const us_osc_msg_element_d_t *e = (const us_osc_msg_element_d_t *)element;
            r &= result->printf(result, " %1.14g", e->m_value);
            break;
        }
        case 'f': {
            const us_osc_msg_element_f_t *e = (const us_osc_msg_element_f_t *)element;
            r &= result->printf(result, " %1.14g", e->m_value);
            break;
        }
        case 'h': {
            const us_osc_msg_element_h_t *e = (const us_osc_msg_element_h_t *)element;
            r &= result->printf(result, " %08x%08x", e->m_value_high, e->m_value_low);
            break;
        }
        case 'i': {
            const us_osc_msg_element_i_t *e = (const us_osc_msg_element_i_t *)element;
            r &= result->printf(result, " %d", e->m_value);
            break;
        }
        case 's': {
            const us_osc_msg_element_s_t *e = (const us_osc_msg_element_s_t *)element;
            int i;
            r &= result->printf(result, " \"");
            for (i = 0; i < e->m_length; ++i) {
                char p = e->m_value[i];
                switch (p) {
                case '\\':
                    r &= result->printf(result, "\\\\");
                    break;
                case '\r':
                    r &= result->printf(result, "\\r");
                    break;
                case '\n':
                    r &= result->printf(result, "\\n");
                    break;
                case '\"':
                    r &= result->printf(result, "\\\"");
                    break;
                case '\'':
                    r &= result->printf(result, "\\\'");
                    break;
                case '\t':
                    r &= result->printf(result, "\\t");
                    break;
                default:
                    if (p < ' ') {
                        r &= result->printf(result, "\\x%02x", p);
                    } else {
                        r &= result->printf(result, "%c", p);
                    }
                    break;
                }
            }
            r &= result->printf(result, "\"");
        }
        case 't': {
            const us_osc_msg_element_t_t *e = (const us_osc_msg_element_t_t *)element;
            r &= result->printf(result, " %08x%08x", e->m_time_high, e->m_time_low);
            break;
        }
        case 'T':
        case 'F':
        case 'I':
        case 'N': { break; }
        case 'M': {
            const us_osc_msg_element_M_t *e = (const us_osc_msg_element_M_t *)element;
            r &= result->printf(result, " { ");
            r &= us_osc_line_gen_msg(e->m_msg, result);
            r &= result->printf(result, " }");
            break;
        }
        }
        element = element->m_next;
    }
    return r;
}

#if 1
bool us_osc_line_gen_bundle(const us_osc_msg_bundle_t *bundle, us_print_t *result) { return false; }
#endif
