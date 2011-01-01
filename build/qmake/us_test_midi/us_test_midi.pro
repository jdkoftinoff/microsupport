! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_midi

SOURCES += $$TOPDIR/tests/us_test_midi.c

HEADERS += $$TOPDIR/include/*.h

