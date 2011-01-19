! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_http_client

SOURCES += $$TOPDIR/examples/us_example_http_client.c

HEADERS += $$TOPDIR/include/*.h

