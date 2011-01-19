! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_http_client

SOURCES += $$TOPDIR/tests/us_test_http_client.c

HEADERS += $$TOPDIR/include/*.h

