TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

SOURCES += tst_theme.cpp

OTHER_FILES += $$system(ls *.qml)
