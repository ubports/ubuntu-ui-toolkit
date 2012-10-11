include( ../../../../qtversioncheck.pri )

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml quick quick-private
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h \
           units.h \
           scalingimageprovider.h \
           qquickimageextension.h

SOURCES += plugin.cpp \
           units.cpp \
           scalingimageprovider.cpp \
           qquickimageextension.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

