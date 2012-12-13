TESTS += $$system(ls tst_*.qml)

include(../qmltest-include.pri)

TARGET = tst_qmltemplate

SOURCES += tst_qmltemplate.cpp

OTHER_FILES += $$system(ls *.qml)
