! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_http_server

SOURCES += $$TOPDIR/examples/us_example_http_server.c

HEADERS += $$TOPDIR/include/*.h

