QT -= core gui
CONFIG += create_prl
TOPDIR= ../../..
INCLUDEPATH += $$TOPDIR/include
DEPENDPATH += $$INCLUDEPATH
TARGET = us
TEMPLATE = lib
CONFIG += staticlib

SOURCES += $$TOPDIR/src/*.cpp

#win32:SOURCES += $$TOPDIR/src/win32/*.cpp
#macx:SOURCES += $$TOPDIR/src/macosx/*.cpp
#unix:SOURCES += $$TOPDIR/src/posix/*.cpp

HEADERS += $$TOPDIR/include/*.hpp

