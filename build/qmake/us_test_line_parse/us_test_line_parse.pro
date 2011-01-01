! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_line_parse

SOURCES += $$TOPDIR/tests/us_test_line_parse.c

HEADERS += $$TOPDIR/include/*.h

