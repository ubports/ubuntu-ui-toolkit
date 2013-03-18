TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_tests

SOURCES += tst_tests.cpp

OTHER_FILES += $$system(ls *.qml)
