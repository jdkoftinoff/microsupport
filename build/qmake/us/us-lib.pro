QT -= core gui
CONFIG += create_prl
TOPDIR= ../../..
INCLUDEPATH += $$TOPDIR/include
DEPENDPATH += $$INCLUDEPATH
TARGET = us
TEMPLATE = lib
CONFIG += staticlib

SOURCES += $$TOPDIR/src/*.c

HEADERS += $$TOPDIR/include/*.h

