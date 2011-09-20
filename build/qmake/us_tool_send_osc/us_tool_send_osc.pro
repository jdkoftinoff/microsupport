! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_tool_send_osc

SOURCES += $$TOPDIR/tools-dev/us_tool_send_osc.cpp

HEADERS += $$TOPDIR/include/*.hpp

