! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_hex_udp_tx

SOURCES += $$TOPDIR/examples/us_example_hex_udp_tx.c

HEADERS += $$TOPDIR/include/*.h

