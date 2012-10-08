#-------------------------------------------------
#
# Project created by QtCreator 2012-10-02T11:28:46
#
#-------------------------------------------------

QT       += testlib

QT       += qml #core gui

TARGET = tst_themeenginetest
CONFIG   += qt console no_keywords
#CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_themeenginetesttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../theme-engine.pri)
