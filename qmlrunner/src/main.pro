TEMPLATE = app
TARGET = ../qmlrunner

QT += widgets quick
INCLUDEPATH += core

SOURCES += \
    main.cpp

OBJECTS_DIR = .obj/

target.path = /usr/bin

INSTALLS += target
