! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_tool_send_osc

SOURCES += $$TOPDIR/tools-dev/us_tool_send_osc.c

HEADERS += $$TOPDIR/include/*.h

