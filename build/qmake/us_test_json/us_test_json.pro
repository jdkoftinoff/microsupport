! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_json

SOURCES += $$TOPDIR/tests/us_test_json.cpp

HEADERS += $$TOPDIR/include/*.hpp

