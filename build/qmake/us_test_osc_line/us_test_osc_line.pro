! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_osc_line

SOURCES += $$TOPDIR/tests/us_test_osc_line.c

HEADERS += $$TOPDIR/include/*.h

