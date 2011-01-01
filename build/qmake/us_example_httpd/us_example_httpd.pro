! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_example_httpd

SOURCES += $$TOPDIR/examples/us_example_httpd.c

HEADERS += $$TOPDIR/include/*.h

