include(../qmltest-include.pri)

TARGET = tst_components

SOURCES += tst_components.cpp

OTHER_FILES += $$system(ls *.qml)
