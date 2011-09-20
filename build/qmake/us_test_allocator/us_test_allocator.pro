! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_allocator

SOURCES += $$TOPDIR/tests/us_test_allocator.cpp

HEADERS += $$TOPDIR/include/*.hpp

