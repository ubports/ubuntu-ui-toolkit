TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_components

CONFIG += c++11

SOURCES += tst_components.cpp tabsmodel.cpp
HEADERS += tabsmodel.h

OTHER_FILES += $$system(ls *.qml)
OTHER_FILES += $$system(ls AppTheme/*)
