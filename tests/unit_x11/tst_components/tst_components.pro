TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_components

SOURCES += tst_components.cpp tabsmodel.cpp
HEADERS += tabsmodel.h

OTHER_FILES += $$system(ls *.qml) \
    tst_ubuntu-namespace_v12.qml
OTHER_FILES += $$system(ls AppTheme/*)
