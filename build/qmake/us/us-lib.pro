QT -= core gui
CONFIG += create_prl
TOPDIR= ../../..
INCLUDEPATH += $$TOPDIR/include
DEPENDPATH += $$INCLUDEPATH
TARGET = us
TEMPLATE = lib
CONFIG += staticlib

SOURCES += $$TOPDIR/src/*.c

#win32:SOURCES += $$TOPDIR/src/win32/*.c
#macx:SOURCES += $$TOPDIR/src/macosx/*.c
#unix:SOURCES += $$TOPDIR/src/posix/*.c

HEADERS += $$TOPDIR/include/*.h

