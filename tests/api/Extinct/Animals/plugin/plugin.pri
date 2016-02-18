QT *= core-private qml qml-private quick quick-private gui-private

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT *= v8-private
}

CONFIG  += no_keywords
DEFINES += UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/qml\\"'
DEFINES += QT_USE_QSTRINGBUILDER

TARGET = $$qtLibraryTarget($$TARGET)
uri = Extinct.Animals

HEADERS += \
    $$PWD/plugin.h \
    $$PWD/tarpan.h \
    $$PWD/smilodon.h \
    $$PWD/pangaea.h \
    $$PWD/rune.h \
    $$PWD/europe.h \


SOURCES += \
    $$PWD/plugin.cpp \
    $$PWD/tarpan.cpp \
    $$PWD/smilodon.cpp \
    $$PWD/pangaea.cpp \
    $$PWD/rune.cpp \
    $$PWD/europe.cpp \

RESOURCES += \
    $$PWD/plugin.qrc
