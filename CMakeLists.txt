cmake_minimum_required (VERSION 2.8) 
project (microsupport)
enable_testing()

set(LIBS ${LIBS} ${CHECK_LIBRARIES} microsupport)

include_directories( include )

add_library (microsupport 
    src/us_allocator.c src/us_allocator_heap.c src/us_allocator_print.c src/us_buffer.c src/us_buffer_print.c src/us_cgi.c src/us_crc32.c src/us_getopt.c src/us_http.c src/us_http_client.c src/us_http_server.c src/us_json.c src/us_json_parser.c src/us_line_parse.c src/us_logger.c src/us_logger_printer.c src/us_logger_stdio.c src/us_logger_syslog.c src/us_logger_udp.c src/us_mdns_avahi.c src/us_midi.c src/us_mtc.c src/us_mtc_parser.c src/us_net.c src/us_osc_dispatch.c src/us_osc_io.c src/us_osc_line.c src/us_osc_msg.c src/us_osc_msg_print.c src/us_osc_sender.c src/us_osc_sender_tcp.c src/us_osc_sender_udp.c src/us_packet.c src/us_packet_cpp.cpp src/us_packet_queue.c src/us_packet_queue_cpp.cpp src/us_parse.c src/us_platform.c src/us_print.c src/us_queue.c src/us_rawnet.c src/us_reactor.c src/us_reactor_cpp.cpp src/us_reactor_handler_rawnet.c src/us_reactor_handler_tcp.c src/us_reactor_handler_udp.c src/us_serial.c src/us_slip.c src/us_test_allocator_heap_main.c src/us_test_allocator_main.c src/us_test_buffer_main.c src/us_test_getopt_main.c src/us_test_http_client_main.c src/us_test_http_main.c src/us_test_http_server_main.c src/us_test_json_main.c src/us_test_line_parse_main.c src/us_test_midi_main.c src/us_test_mtc_main.c src/us_test_net_main.c src/us_test_osc_dispatch_main.c src/us_test_osc_io_main.c src/us_test_osc_line_main.c src/us_test_osc_msg_main.c src/us_test_packet_queue_main.c src/us_test_queue_main.c src/us_test_slip_main.c src/us_test_trie_main.c src/us_test_webapp_main.c src/us_testutil.c src/us_time.c src/us_trie_dump.c src/us_trie_ro.c src/us_trie_rw.c src/us_webapp.c src/us_webapp_fs.c src/us_webapp_json.c src/us_world.c include/us.h include/us_allocator.h include/us_allocator_heap.h include/us_allocator_print.h include/us_bits.h include/us_buffer.h include/us_buffer_print.h include/us_cgi.h include/us_crc32.h include/us_daemon.h include/us_getopt.h include/us_http.h include/us_http_client.h include/us_http_server.h include/us_json.h include/us_json_parser.h include/us_line_parse.h include/us_logger.h include/us_logger_printer.h include/us_logger_stdio.h include/us_logger_syslog.h include/us_logger_udp.h include/us_mdns_avahi.h include/us_midi.h include/us_mtc.h include/us_mtc_parser.h include/us_net.h include/us_options.h include/us_osc_dispatch.h include/us_osc_io.h include/us_osc_line.h include/us_osc_msg.h include/us_osc_msg_print.h include/us_osc_sender.h include/us_osc_sender_tcp.h include/us_osc_sender_udp.h include/us_packet.h include/us_packet.hpp include/us_packet_queue.h include/us_packet_queue.hpp include/us_parse.h include/us_platform.h include/us_print.h include/us_queue.h include/us_rawnet.h include/us_reactor.h include/us_reactor.hpp include/us_reactor_handler_rawnet.h include/us_reactor_handler_tcp.h include/us_reactor_handler_udp.h include/us_serial.h include/us_slip.h include/us_test_allocator_heap_main.h include/us_test_allocator_main.h include/us_test_buffer_main.h include/us_test_getopt_main.h include/us_test_http_client_main.h include/us_test_http_main.h include/us_test_http_server_main.h include/us_test_json_main.h include/us_test_line_parse_main.h include/us_test_midi_main.h include/us_test_mtc_main.h include/us_test_net_main.h include/us_test_osc_dispatch_main.h include/us_test_osc_io_main.h include/us_test_osc_line_main.h include/us_test_osc_msg_main.h include/us_test_packet_queue_main.h include/us_test_queue_main.h include/us_test_slip_main.h include/us_test_trie_main.h include/us_test_webapp_main.h include/us_testutil.h include/us_time.h include/us_trie.h include/us_trie_dump.h include/us_webapp.h include/us_webapp_fs.h include/us_webapp_json.h include/us_world.h include/us_world.hpp
)

INSTALL(TARGETS microsupport DESTINATION "lib")

INSTALL(FILES
    include/us.h include/us_allocator.h include/us_allocator_heap.h include/us_allocator_print.h include/us_bits.h include/us_buffer.h include/us_buffer_print.h include/us_cgi.h include/us_crc32.h include/us_daemon.h include/us_getopt.h include/us_http.h include/us_http_client.h include/us_http_server.h include/us_json.h include/us_json_parser.h include/us_line_parse.h include/us_logger.h include/us_logger_printer.h include/us_logger_stdio.h include/us_logger_syslog.h include/us_logger_udp.h include/us_mdns_avahi.h include/us_midi.h include/us_mtc.h include/us_mtc_parser.h include/us_net.h include/us_options.h include/us_osc_dispatch.h include/us_osc_io.h include/us_osc_line.h include/us_osc_msg.h include/us_osc_msg_print.h include/us_osc_sender.h include/us_osc_sender_tcp.h include/us_osc_sender_udp.h include/us_packet.h include/us_packet.hpp include/us_packet_queue.h include/us_packet_queue.hpp include/us_parse.h include/us_platform.h include/us_print.h include/us_queue.h include/us_rawnet.h include/us_reactor.h include/us_reactor.hpp include/us_reactor_handler_rawnet.h include/us_reactor_handler_tcp.h include/us_reactor_handler_udp.h include/us_serial.h include/us_slip.h include/us_test_allocator_heap_main.h include/us_test_allocator_main.h include/us_test_buffer_main.h include/us_test_getopt_main.h include/us_test_http_client_main.h include/us_test_http_main.h include/us_test_http_server_main.h include/us_test_json_main.h include/us_test_line_parse_main.h include/us_test_midi_main.h include/us_test_mtc_main.h include/us_test_net_main.h include/us_test_osc_dispatch_main.h include/us_test_osc_io_main.h include/us_test_osc_line_main.h include/us_test_osc_msg_main.h include/us_test_packet_queue_main.h include/us_test_queue_main.h include/us_test_slip_main.h include/us_test_trie_main.h include/us_test_webapp_main.h include/us_testutil.h include/us_time.h include/us_trie.h include/us_trie_dump.h include/us_webapp.h include/us_webapp_fs.h include/us_webapp_json.h include/us_world.h include/us_world.hpp

   DESTINATION include
)

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

