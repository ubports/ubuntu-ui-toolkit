TEMPLATE = app
QT += qml quick
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
HEADERS += MouseTouchAdaptor.h
SOURCES += launcher.cpp \
    MouseTouchAdaptor.cpp
launcher.path = /usr/lib/ubuntu-ui-toolkit
launcher.files = launcher
INSTALLS += launcher

