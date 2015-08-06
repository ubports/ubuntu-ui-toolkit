TEMPLATE = app
QT += qml quick
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
HEADERS += MouseTouchAdaptor.h
SOURCES += launcher.cpp \
    MouseTouchAdaptor.cpp
installPath = $$[QT_INSTALL_PREFIX]/bin
launcher.path = $$installPath
launcher.files = ubuntu-ui-toolkit-launcher
INSTALLS += launcher

