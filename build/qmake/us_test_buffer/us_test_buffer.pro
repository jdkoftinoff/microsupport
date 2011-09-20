! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_buffer

SOURCES += $$TOPDIR/tests/us_test_buffer.cpp

HEADERS += $$TOPDIR/include/*.hpp

