TESTS += $$system(ls tst_*.qml)

include(../qmltest-include-x11.pri)

CONFIG += c++11

SOURCES += tst_visual.cpp tabsmodel.cpp
HEADERS += tabsmodel.h

OTHER_FILES += $$system(ls *.qml)
OTHER_FILES += $$system(ls AppTheme/*)

check.commands += $$PWD/naming.sh
