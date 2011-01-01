TOPDIR= ../../..

QT -= core gui
CONFIG += link_prl
macx:CONFIG-=app_bundle

TEMPLATE = app

INCLUDEPATH += $$TOPDIR/include 
DEPENDPATH += $$INCLUDEPATH

LIBS += -L../us -lus

