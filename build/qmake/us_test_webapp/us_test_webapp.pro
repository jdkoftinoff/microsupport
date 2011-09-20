! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_webapp

SOURCES += $$TOPDIR/tests/us_test_webapp.cpp

HEADERS += $$TOPDIR/include/*.hpp

