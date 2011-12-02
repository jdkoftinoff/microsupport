! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_allocator_heap

SOURCES += $$TOPDIR/tests/us_test_allocator_heap.c

HEADERS += $$TOPDIR/include/*.h

