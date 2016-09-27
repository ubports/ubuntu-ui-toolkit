TEMPLATE = app
QT += \
    core-private \
    gui-private \
    quick \
    quick-private \
    qml \
    testlib \
    UbuntuToolkit \
    UbuntuToolkit_private \
    UbuntuMetrics
CONFIG += no_keywords c++11
SOURCES += launcher.cpp
installPath = $$[QT_INSTALL_PREFIX]/bin
launcher.path = $$installPath
launcher.files = ubuntu-ui-toolkit-launcher
INSTALLS += launcher
