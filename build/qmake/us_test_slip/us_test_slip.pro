! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_slip

SOURCES += $$TOPDIR/tests/us_test_slip.c

HEADERS += $$TOPDIR/include/*.h

