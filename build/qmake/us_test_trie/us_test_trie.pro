! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=us_test_trie

SOURCES += $$TOPDIR/tests/us_test_trie.c

HEADERS += $$TOPDIR/include/*.h

