! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_hex_udp_rx

SOURCES += $$TOPDIR/examples/us_example_hex_udp_rx.c

HEADERS += $$TOPDIR/include/*.h

