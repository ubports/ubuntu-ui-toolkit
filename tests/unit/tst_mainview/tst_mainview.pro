include(../test-include.pri)

QT += gui
SOURCES += tst_mainview.cpp

OTHER_FILES += $$system(ls tst_*.qml)
