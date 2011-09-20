! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_tool_rx_osc

SOURCES += $$TOPDIR/tools-dev/us_tool_rx_osc.cpp

HEADERS += $$TOPDIR/include/*.hpp

