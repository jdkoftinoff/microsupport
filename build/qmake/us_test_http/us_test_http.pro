! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_http

SOURCES += $$TOPDIR/tests/us_test_http.c

HEADERS += $$TOPDIR/include/*.h

