TEMPLATE = app
QT += qml quick
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
HEADERS += MouseTouchAdaptor.h
SOURCES += launcher.cpp \
    MouseTouchAdaptor.cpp
installPath = $$[QT_INSTALL_LIBS]/ubuntu-ui-toolkit
launcher.path = $$installPath
launcher.files = launcher
INSTALLS += launcher

