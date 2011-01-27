! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_webapp

SOURCES += $$TOPDIR/examples/us_example_webapp.c

HEADERS += $$TOPDIR/include/*.h

