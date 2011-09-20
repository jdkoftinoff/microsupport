! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_http_server

SOURCES += $$TOPDIR/examples/us_example_http_server.cpp

HEADERS += $$TOPDIR/include/*.hpp

