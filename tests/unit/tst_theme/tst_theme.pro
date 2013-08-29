TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_theme

SOURCES += tst_theme.cpp

OTHER_FILES += $$system(ls *.qml)
