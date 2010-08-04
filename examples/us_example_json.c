/* us_example_json.c */

/*
    This program demonstrates a simple application of JSON_parser. It reads
    a JSON text from STDIN, producing an error message if the text is rejected.

        % us_example_json <test/pass1.json
*/

#include "us_world.h"
#include "us_allocator.h"
#include "us_json_parser.h"

#ifdef US_CONFIG_MICROCONTROLLER

int main()
{
  return 0;
}

#else

static int print(void* ctx, int type, const us_json_value_t* value);

int main(int argc, char* argv[])
{
  int count = 0, result = 0;
  FILE* input;
  us_malloc_allocator_t malloc_allocator;
  us_allocator_t *allocator;
  us_json_config_t config;
  us_json_parser_t jc = NULL;

  allocator = us_malloc_allocator_init( &malloc_allocator );

  us_json_config_init(&config);

  config.depth                  = 20;
  config.callback               = &print;
  config.allow_comments         = 1;
  config.handle_floats_manually = 0;

  /* Important! Set locale before parser is created.*/
#if 0
  if (argc >= 2) {
    if (!setlocale(LC_ALL, argv[1])) {
      fprintf(stderr, "Failed to set locale to '%s'\n", argv[1]);
    }
  } else {
    fprintf(stderr, "No locale provided, C locale is used\n");
  }
#endif

  jc = us_json_parser_create(allocator,&config);

  if( argc>1 )
  {
    input = fopen( argv[1], "rt" );
  }
  else
  {
    input = stdin;
  }
  for (; input ; ++count) {
    int next_char = fgetc(input);
    if (next_char <= 0) {
      break;
    }
    if (!us_json_parser_char(jc, next_char)) {
      fprintf(stderr, "JSON_parser_char: syntax error, byte %d\n", count);
      result = 1;
      goto done;
    }
  }
  if (!us_json_parser_done(jc)) {
    fprintf(stderr, "JSON_parser_end: syntax error\n");
    result = 1;
    goto done;
  }

 done:
  us_json_parser_destroy(jc);
  if( input != stdin )
    fclose(input);
  return result;
}

static size_t s_Level = 0;

static const char* s_pIndention = "  ";

static int s_IsKey = 0;

static void print_indention()
{
  size_t i;

  for (i = 0; i < s_Level; ++i) {
    printf("%s", s_pIndention);
  }
}


static int print(void* ctx, int type, const us_json_value_t* value)
{
  switch(type) {
    case US_JSON_T_ARRAY_BEGIN:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("[\n");
      ++s_Level;
      break;
    case US_JSON_T_ARRAY_END:
      assert(!s_IsKey);
      if (s_Level > 0) --s_Level;
      print_indention();
      printf("]\n");
      break;
    case US_JSON_T_OBJECT_BEGIN:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("{\n");
      ++s_Level;
      break;
    case US_JSON_T_OBJECT_END:
      assert(!s_IsKey);
      if (s_Level > 0) --s_Level;
      print_indention();
      printf("}\n");
      break;
    case US_JSON_T_INTEGER:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("integer: "US_JSON_PARSER_INTEGER_SPRINTF_TOKEN"\n", value->vu.integer_value);
      break;
    case US_JSON_T_FLOAT:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("float: %f\n", value->vu.float_value); /* We wanted stringified floats */
      break;
    case US_JSON_T_NULL:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("null\n");
      break;
    case US_JSON_T_TRUE:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("true\n");
      break;
    case US_JSON_T_FALSE:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("false\n");
      break;
    case US_JSON_T_KEY:
      s_IsKey = 1;
      print_indention();
      printf("key = '%s', value = ", value->vu.str.value);
      break;
    case US_JSON_T_STRING:
      if (!s_IsKey) print_indention();
      s_IsKey = 0;
      printf("string: '%s'\n", value->vu.str.value);
      break;
    default:
      assert(0);
      break;
  }

  return 1;
}


#endif
