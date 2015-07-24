QT *= core-private qml qml-private quick quick-private gui-private testlib

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT *= v8-private
}

CONFIG  += no_keywords
DEFINES += UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/qml\\"'

QMAKE_CXXFLAGS += -Werror

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Test

HEADERS += \
    $$PWD/uctestcase.h \
    $$PWD/testplugin.h \
    $$PWD/uctestextras.h

SOURCES += \
    $$PWD/uctestcase.cpp \
    $$PWD/testplugin.cpp \
    $$PWD/uctestextras.cpp
