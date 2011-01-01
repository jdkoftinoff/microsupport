! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_json

SOURCES += $$TOPDIR/examples/us_example_json.c

HEADERS += $$TOPDIR/include/*.h

