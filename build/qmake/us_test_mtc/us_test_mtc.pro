! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_mtc

SOURCES += $$TOPDIR/tests/us_test_mtc.c

HEADERS += $$TOPDIR/include/*.h

