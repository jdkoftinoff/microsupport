! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_all

SOURCES += $$TOPDIR/tests/us_test_all.cpp

HEADERS += $$TOPDIR/include/*.hpp

