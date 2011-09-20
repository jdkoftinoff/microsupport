! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_mudp_rx

SOURCES += $$TOPDIR/examples/us_example_mudp_rx.cpp

HEADERS += $$TOPDIR/include/*.hpp

