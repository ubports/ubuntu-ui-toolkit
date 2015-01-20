TEMPLATE = lib
TARGET = ../UbuntuTest
QT += core-private qml qml-private quick quick-private gui-private testlib

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG  += qt plugin no_keywords
DEFINES += UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/modules\\"'

QMAKE_CXXFLAGS += -Werror

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Test

HEADERS += \
    uctestcase.h \
    testplugin.h \
    uctestextras.h

SOURCES += \
    uctestcase.cpp \
    testplugin.cpp \
    uctestextras.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
