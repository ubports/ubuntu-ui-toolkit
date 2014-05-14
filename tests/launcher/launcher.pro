TEMPLATE = app
QT += qml quick
# For setSharedOpenGLContext
QT += core-private gui-private quick-private
CONFIG += no_keywords
SOURCES += \
    launcher.cpp
launcher.path = /usr/lib/ubuntu-ui-toolkit
launcher.files = launcher
INSTALLS += launcher

