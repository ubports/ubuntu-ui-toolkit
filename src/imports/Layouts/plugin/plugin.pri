QT *= core-private gui-private quick-private qml-private
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT *= v8-private
}

CONFIG += no_keywords

QMAKE_CXXFLAGS += -Werror

DEFINES += QT_USE_QSTRINGBUILDER

# Input
SOURCES += \
    $$PWD/ullayouts.cpp \
    $$PWD/ulconditionallayout.cpp \
    $$PWD/ulplugin.cpp \
    $$PWD/ulitemlayout.cpp \
    $$PWD/ullayoutsattached.cpp \
    $$PWD/propertychanges.cpp

HEADERS += \
    $$PWD/ullayouts.h \
    $$PWD/ullayouts_p.h \
    $$PWD/ulconditionallayout.h \
    $$PWD/ulconditionallayout_p.h \
    $$PWD/ulplugin.h \
    $$PWD/ulitemlayout.h \
    $$PWD/propertychanges_p.h
