! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_reactor

SOURCES += $$TOPDIR/examples/us_example_reactor.c

HEADERS += $$TOPDIR/include/*.h

