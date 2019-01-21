TARGET = qcustom
TEMPLATE = lib

CONFIG += plugin no_keywords c++11

QT += core-private gui-private eventdispatcher_support-private

SOURCES =   main.cpp \
            qcustomintegration.cpp \
            qcustombackingstore.cpp
HEADERS =   qcustomintegration.h \
            qcustombackingstore.h

OTHER_FILES += custom.json
