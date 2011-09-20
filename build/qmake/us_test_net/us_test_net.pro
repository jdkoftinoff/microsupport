! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_net

SOURCES += $$TOPDIR/tests/us_test_net.cpp

HEADERS += $$TOPDIR/include/*.hpp

