TARGET = qcustom
TEMPLATE = lib

CONFIG += plugin no_keywords c++11

QT -= gui
QT += core-private gui-private platformsupport-private

SOURCES =   main.cpp \
            qcustomintegration.cpp \
            qcustombackingstore.cpp
HEADERS =   qcustomintegration.h \
            qcustombackingstore.h

OTHER_FILES += custom.json
