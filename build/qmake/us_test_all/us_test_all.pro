! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_all

SOURCES += $$TOPDIR/tests/us_test_all.c

HEADERS += $$TOPDIR/include/*.h

