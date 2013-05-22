include( ../../../../common.pri )

TEMPLATE = lib
TARGET = ../UbuntuLayouts
QT += qml quick
CONFIG += qt plugin no_keywords

#QT += quick-private
QT += qml-private core-private v8-private

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Layouts

# Input
SOURCES += \
    ullayouts.cpp \
    layoutsplugin.cpp \
    ulconditionallayout.cpp \
    ulconditionallayoutattached.cpp

HEADERS += \
    ullayouts.h \
    layoutsplugin.h \
    ullayouts_p.h \
    ulconditionallayout.h \
    ulconditionallayout_p.h

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
