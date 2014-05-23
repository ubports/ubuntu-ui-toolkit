TEMPLATE = lib
TARGET = ../UbuntuLayouts
QT += qml quick
CONFIG += qt plugin no_keywords

QMAKE_CXXFLAGS += -Werror

QT += quick-private gui-private
QT += qml-private core-private

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Layouts

# Input
SOURCES += \
    ullayouts.cpp \
    ulconditionallayout.cpp \
    ulplugin.cpp \
    ulitemlayout.cpp \
    ullayoutsattached.cpp \
    propertychanges.cpp

HEADERS += \
    ullayouts.h \
    ullayouts_p.h \
    ulconditionallayout.h \
    ulconditionallayout_p.h \
    ulplugin.h \
    ulitemlayout.h \
    propertychanges_p.h

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
