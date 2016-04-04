TEMPLATE = app
QT += qml quick
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private UbuntuToolkit
CONFIG += no_keywords
SOURCES += launcher.cpp
installPath = $$[QT_INSTALL_PREFIX]/bin
launcher.path = $$installPath
launcher.files = ubuntu-ui-toolkit-launcher
INSTALLS += launcher

