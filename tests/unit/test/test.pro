TESTS += $$system(ls tst_*.qml)

include(../qmltest-include-x11.pri)

SOURCES += tst_test.cpp

OTHER_FILES += $$system(ls *.qml)
