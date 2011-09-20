! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_tool_create_trie

SOURCES += $$TOPDIR/tools-dev/us_tool_create_trie.cpp

HEADERS += $$TOPDIR/include/*.hpp

