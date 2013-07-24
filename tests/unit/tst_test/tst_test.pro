TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_test

SOURCES += tst_test.cpp

OTHER_FILES += $$system(ls *.qml)
