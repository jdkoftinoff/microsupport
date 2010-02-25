QT -= core gui
CONFIG += create_prl
TOPDIR= ../../..

INCLUDEPATH += $$TOPDIR/include 
DEPENDPATH += $$INCLUDEPATH
TEMPLATE = app
CONFIG += debug

LIBS += -L../us -lus

TEST = us_test_allocator
TARGET = $${TEST}

SOURCES += $$TOPDIR/tests/$${TEST}.c

HEADERS += $$TOPDIR/include/*.h

