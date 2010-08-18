/*
  Copyright (c) 2005 JSON.org with modifications by Jeff Koftinoff <jeffk@jdkoftinoff.com> 2010

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  The Software shall be used for Good, not Evil.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef US_JSON_PARSER_H
#define US_JSON_PARSER_H

#include "us_world.h"
#include "us_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif


    /* Determine the integer type use to parse non-floating point numbers */
#if __STDC_VERSION__ >= 199901L || HAVE_LONG_LONG == 1
    typedef long long us_json_int_t;
#define US_JSON_PARSER_INTEGER_SSCANF_TOKEN "%lld"
#define US_JSON_PARSER_INTEGER_SPRINTF_TOKEN "%lld"
#else
    typedef long us_json_int_t;
#define US_JSON_PARSER_INTEGER_SSCANF_TOKEN "%ld"
#define US_JSON_PARSER_INTEGER_SPRINTF_TOKEN "%ld"
#endif
    
    
    typedef enum
    {
        US_JSON_T_NONE = 0,
        US_JSON_T_ARRAY_BEGIN,
        US_JSON_T_ARRAY_END,
        US_JSON_T_OBJECT_BEGIN,
        US_JSON_T_OBJECT_END,
        US_JSON_T_INTEGER,
        US_JSON_T_FLOAT,
        US_JSON_T_NULL,
        US_JSON_T_TRUE,
        US_JSON_T_FALSE,
        US_JSON_T_STRING,
        US_JSON_T_KEY,
        US_JSON_T_MAX
    } us_json_type;
    
    typedef struct us_json_value_struct
    {
        union
        {
            us_json_int_t integer_value;
            
            double float_value;
            
            struct
            {
                const char* value;
                size_t length;
            } str;
        } vu;
    } us_json_value_t;
    
    typedef struct us_json_parser_struct* us_json_parser_t;
    
    /*! \brief JSON parser callback
    
      \param ctx The pointer passed to new_us_json_parser.
      \param type An element of us_json_type but not US_JSON_T_NONE.
      \param value A representation of the parsed value. This parameter is NULL for
      US_JSON_T_ARRAY_BEGIN, US_JSON_T_ARRAY_END, US_JSON_T_OBJECT_BEGIN, US_JSON_T_OBJECT_END,
      US_JSON_T_NULL, US_JSON_T_TRUE, and SON_T_FALSE. String values are always returned
      as zero-terminated C strings.
    
      \return Non-zero if parsing should continue, else zero.
    */
    typedef int ( *us_json_parser_callback ) ( void* ctx, int type, const struct us_json_value_struct* value );
    
    
    /*! \brief The structure used to configure a JSON parser object
    
      \param depth If negative, the parser can parse arbitrary levels of JSON, otherwise
      the depth is the limit
      \param Pointer to a callback. This parameter may be NULL. In this case the input is merely checked for validity.
      \param Callback context. This parameter may be NULL.
      \param depth. Specifies the levels of nested JSON to allow. Negative numbers yield unlimited nesting.
      \param allowComments. To allow C style comments in JSON, set to non-zero.
      \param handleFloatsManually. To decode floating point numbers manually set this parameter to non-zero.
    
      \return The parser object.
    */
    typedef struct
    {
        us_json_parser_callback     callback;
        void*                    callback_ctx;
        int                      depth;
        int                      allow_comments;
        int                      handle_floats_manually;
    } us_json_config_t;
    
    
    /*! \brief Initializes the JSON parser configuration structure to default values.
    
      The default configuration is
      - 127 levels of nested JSON (depends on JSON_PARSER_STACK_SIZE, see json_parser.c)
      - no parsing, just checking for JSON syntax
      - no comments
    
      \param config Used to configure the parser.
    */
    void us_json_config_init ( us_json_config_t* config );
    
    /*! \brief Create a JSON parser object
    
      \param allocator The memory allocator that the json object will use to allocate/deallocate memory
      \param config Used to configure the parser. Set to NULL to use the default configuration.
      See init_json_config_t
    
      \return The parser object.
    */
    extern us_json_parser_t us_json_parser_create ( us_allocator_t *allocator, us_json_config_t* config );
    
    /*! \brief Destroy a previously created JSON parser object. */
    extern void us_json_parser_destroy ( us_json_parser_t jc );
    
    /*! \brief Parse a character.
    
      \return Non-zero, if all characters passed to this function are part of are valid JSON.
    */
    extern int us_json_parser_char ( us_json_parser_t jc, int next_char );
    
    /*! \brief Finalize parsing.
    
      Call this method once after all input characters have been consumed.
    
      \return Non-zero, if all parsed characters are valid JSON, zero otherwise.
    */
    extern int us_json_parser_done ( us_json_parser_t jc );
    
    /*! \brief Determine if a given string is valid JSON white space
    
      \return Non-zero if the string is valid, zero otherwise.
    */
    extern int us_json_parser_is_legal_white_space_string ( const char* s );
    
    
#ifdef __cplusplus
}
#endif


#endif

