! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_osc_msg

SOURCES += $$TOPDIR/tests/us_test_osc_msg.c

HEADERS += $$TOPDIR/include/*.h

