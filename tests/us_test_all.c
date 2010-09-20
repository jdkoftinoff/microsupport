#include "us_world.h"

#include "us_test_allocator_main.h"
#include "us_test_buffer_main.h"
#include "us_test_http_main.h"
#include "us_test_json_main.h"
#include "us_test_line_parse_main.h"
#include "us_test_midi_main.h"
#include "us_test_net_main.h"
#include "us_test_osc_msg_main.h"
#include "us_test_queue_main.h"
#include "us_test_trie_main.h"
#include "us_test_osc_dispatch_main.h"

int main(int argc, char * argv[])
{
    int r=0;
    r += us_test_allocator_main(argc,argv);
    r += us_test_buffer_main(argc, argv );
    r += us_test_http_main(argc, argv );
    r += us_test_json_main(argc, argv );
    r += us_test_line_parse_main(argc, argv );
    r += us_test_midi_main(argc, argv );
    r += us_test_net_main(argc, argv );
    r += us_test_osc_msg_main(argc, argv );
    r += us_test_queue_main(argc, argv );
    r += us_test_osc_dispatch_main(argc,argv );
    r += us_test_trie_main(argc,argv);
    return r;
}

