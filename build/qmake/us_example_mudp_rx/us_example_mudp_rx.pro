! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_mudp_rx

SOURCES += $$TOPDIR/examples/us_example_mudp_rx.c

HEADERS += $$TOPDIR/include/*.h

