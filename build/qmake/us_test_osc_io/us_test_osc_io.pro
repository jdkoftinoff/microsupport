! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_osc_io

SOURCES += $$TOPDIR/tests/us_test_osc_io.cpp

HEADERS += $$TOPDIR/include/*.hpp

