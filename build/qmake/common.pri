TOPDIR= ../../..

QT -= core gui
CONFIG += link_prl
macx:CONFIG-=app_bundle
win32:QT += core
win32:CONFIG+=console

TEMPLATE = app

INCLUDEPATH += $$TOPDIR/include 
DEPENDPATH += $$INCLUDEPATH

!win32:LIBS += -L../us -lus
win32 {
  CONFIG(release):LIBS += -L../us/release -lus
  CONFIG(debug):LIBS += -L../us/debug -lus
}

win32:LIBS+=ws2_32.lib


