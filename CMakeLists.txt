cmake_minimum_required (VERSION 2.8) 
project (microsupport)
enable_testing()

set(LIBS ${LIBS} ${CHECK_LIBRARIES} microsupport)

include_directories( include )

file(GLOB_RECURSE MICROSUPPORT_INCLUDES "include/*.h" "include/*/*.h"  )

if(APPLE)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/win32/*.c" )
endif(APPLE)
if(UNIX)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/posix/*.c"  "src/posix/*.h" )
endif(UNIX)
if(WIN32)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/macosx/*.c" )
endif(WIN32)

add_library (microsupport ${MICROSUPPORT_SRC} ${MICROSUPPORT_INCLUDES})

INSTALL(FILES
        ${MICROSUPPORT_INCLUDES}
        DESTINATION include
)

INSTALL(TARGETS microsupport DESTINATION "lib")

link_directories( ${MICROSUPPORT_BINARY_DIR} )

add_executable(us_test_all tests/us_test_all.c)
target_link_libraries(us_test_all ${LIBS})
add_test(us_test_all ${CMAKE_CURRENT_BINARY_DIR}/us_test_all)

add_executable(us_test_allocator tests/us_test_allocator.c)
target_link_libraries(us_test_allocator ${LIBS})
add_test(us_test_allocator ${CMAKE_CURRENT_BINARY_DIR}/us_test_allocator)

add_executable(us_test_allocator_heap tests/us_test_allocator_heap.c)
target_link_libraries(us_test_allocator_heap ${LIBS})
add_test(us_test_allocator_heap ${CMAKE_CURRENT_BINARY_DIR}/us_test_allocator_heap)

add_executable(us_test_buffer tests/us_test_buffer.c)
target_link_libraries(us_test_buffer ${LIBS})
add_test(us_test_buffer ${CMAKE_CURRENT_BINARY_DIR}/us_test_buffer)

add_executable(us_test_getopt tests/us_test_getopt.c)
target_link_libraries(us_test_getopt ${LIBS})
add_test(us_test_getopt ${CMAKE_CURRENT_BINARY_DIR}/us_test_getopt)

add_executable(us_test_http tests/us_test_http.c)
target_link_libraries(us_test_http ${LIBS})
add_test(us_test_http ${CMAKE_CURRENT_BINARY_DIR}/us_test_http)

add_executable(us_test_http_client tests/us_test_http_client.c)
target_link_libraries(us_test_http_client ${LIBS})
add_test(us_test_http_client ${CMAKE_CURRENT_BINARY_DIR}/us_test_http_client)

add_executable(us_test_http_server tests/us_test_http_server.c)
target_link_libraries(us_test_http_server ${LIBS})
add_test(us_test_http_server ${CMAKE_CURRENT_BINARY_DIR}/us_test_http_server)

add_executable(us_test_json tests/us_test_json.c)
target_link_libraries(us_test_json ${LIBS})
add_test(us_test_json ${CMAKE_CURRENT_BINARY_DIR}/us_test_json)

add_executable(us_test_line_parse tests/us_test_line_parse.c)
target_link_libraries(us_test_line_parse ${LIBS})
add_test(us_test_line_parse ${CMAKE_CURRENT_BINARY_DIR}/us_test_line_parse)

add_executable(us_test_midi tests/us_test_midi.c)
target_link_libraries(us_test_midi ${LIBS})
add_test(us_test_midi ${CMAKE_CURRENT_BINARY_DIR}/us_test_midi)

add_executable(us_test_mtc tests/us_test_mtc.c)
target_link_libraries(us_test_mtc ${LIBS})
add_test(us_test_mtc ${CMAKE_CURRENT_BINARY_DIR}/us_test_mtc)

add_executable(us_test_net tests/us_test_net.c)
target_link_libraries(us_test_net ${LIBS})
add_test(us_test_net ${CMAKE_CURRENT_BINARY_DIR}/us_test_net)

add_executable(us_test_osc_dispatch tests/us_test_osc_dispatch.c)
target_link_libraries(us_test_osc_dispatch ${LIBS})
add_test(us_test_osc_dispatch ${CMAKE_CURRENT_BINARY_DIR}/us_test_osc_dispatch)

add_executable(us_test_osc_io tests/us_test_osc_io.c)
target_link_libraries(us_test_osc_io ${LIBS})
add_test(us_test_osc_io ${CMAKE_CURRENT_BINARY_DIR}/us_test_osc_io)

add_executable(us_test_osc_line tests/us_test_osc_line.c)
target_link_libraries(us_test_osc_line ${LIBS})
add_test(us_test_osc_line ${CMAKE_CURRENT_BINARY_DIR}/us_test_osc_line)

add_executable(us_test_osc_msg tests/us_test_osc_msg.c)
target_link_libraries(us_test_osc_msg ${LIBS})
add_test(us_test_osc_msg ${CMAKE_CURRENT_BINARY_DIR}/us_test_osc_msg)

add_executable(us_test_packet_queue tests/us_test_packet_queue.c)
target_link_libraries(us_test_packet_queue ${LIBS})
add_test(us_test_packet_queue ${CMAKE_CURRENT_BINARY_DIR}/us_test_packet_queue)

add_executable(us_test_queue tests/us_test_queue.c)
target_link_libraries(us_test_queue ${LIBS})
add_test(us_test_queue ${CMAKE_CURRENT_BINARY_DIR}/us_test_queue)

add_executable(us_test_slip tests/us_test_slip.c)
target_link_libraries(us_test_slip ${LIBS})
add_test(us_test_slip ${CMAKE_CURRENT_BINARY_DIR}/us_test_slip)

add_executable(us_test_trie tests/us_test_trie.c)
target_link_libraries(us_test_trie ${LIBS})
add_test(us_test_trie ${CMAKE_CURRENT_BINARY_DIR}/us_test_trie)

add_executable(us_test_webapp tests/us_test_webapp.c)
target_link_libraries(us_test_webapp ${LIBS})
add_test(us_test_webapp ${CMAKE_CURRENT_BINARY_DIR}/us_test_webapp)

add_executable(us_tool_create_trie tools-dev/us_tool_create_trie.c)
target_link_libraries(us_tool_create_trie ${LIBS})

add_executable(us_tool_rx_osc tools-dev/us_tool_rx_osc.c)
target_link_libraries(us_tool_rx_osc ${LIBS})

add_executable(us_tool_send_osc tools-dev/us_tool_send_osc.c)
target_link_libraries(us_tool_send_osc ${LIBS})

add_executable(recv_raw examples/recv_raw.c)
target_link_libraries(recv_raw ${LIBS})

add_executable(send_raw examples/send_raw.c)
target_link_libraries(send_raw ${LIBS})

add_executable(us_example_hex_udp_rx examples/us_example_hex_udp_rx.c)
target_link_libraries(us_example_hex_udp_rx ${LIBS})

add_executable(us_example_hex_udp_tx examples/us_example_hex_udp_tx.c)
target_link_libraries(us_example_hex_udp_tx ${LIBS})

add_executable(us_example_http_client examples/us_example_http_client.c)
target_link_libraries(us_example_http_client ${LIBS})

add_executable(us_example_http_server examples/us_example_http_server.c)
target_link_libraries(us_example_http_server ${LIBS})

add_executable(us_example_json examples/us_example_json.c)
target_link_libraries(us_example_json ${LIBS})

add_executable(us_example_mudp_rx examples/us_example_mudp_rx.c)
target_link_libraries(us_example_mudp_rx ${LIBS})

add_executable(us_example_mudp_tx examples/us_example_mudp_tx.c)
target_link_libraries(us_example_mudp_tx ${LIBS})

add_executable(us_example_reactor examples/us_example_reactor.c)
target_link_libraries(us_example_reactor ${LIBS})

add_executable(us_example_webapp examples/us_example_webapp.c)
target_link_libraries(us_example_webapp ${LIBS})

