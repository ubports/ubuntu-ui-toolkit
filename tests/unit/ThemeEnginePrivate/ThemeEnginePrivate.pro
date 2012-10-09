#-------------------------------------------------
#
# Project created by QtCreator 2012-10-03T15:41:00
#
#-------------------------------------------------

QT       += testlib

QT       += quick qml

TARGET = tst_themeengineprivatetest
CONFIG   += qt console no_keywords
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_themeengineprivatetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../theme-engine.pri)
