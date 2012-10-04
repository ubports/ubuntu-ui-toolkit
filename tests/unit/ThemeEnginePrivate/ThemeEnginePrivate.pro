#-------------------------------------------------
#
# Project created by QtCreator 2012-10-03T15:41:00
#
#-------------------------------------------------

QT       += testlib

QT       += qml core gui

TARGET = tst_themeengineprivatetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_themeengineprivatetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../theme-engine.pri)
