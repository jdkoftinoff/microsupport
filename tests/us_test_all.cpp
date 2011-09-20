#include "us_world.hpp"

#include "us_test_allocator_main.hpp"
#include "us_test_buffer_main.hpp"
#include "us_test_http_main.hpp"
#include "us_test_json_main.hpp"
#include "us_test_line_parse_main.hpp"
#include "us_test_midi_main.hpp"
#include "us_test_net_main.hpp"
#include "us_test_osc_msg_main.hpp"
#include "us_test_queue_main.hpp"
#include "us_test_trie_main.hpp"
#include "us_test_osc_dispatch_main.hpp"

int main(int argc, const char * argv[])
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

