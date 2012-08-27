QT += declarative testlib opengl

TARGET = ../testcase

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    tst_manualtestcase.cpp \
    testcaseexecutiondialog.cpp

HEADERS += \
    tst_manualtestcase.h \
    testcaseexecutiondialog.h

FORMS += \
    testcaseexecutiondialog.ui
