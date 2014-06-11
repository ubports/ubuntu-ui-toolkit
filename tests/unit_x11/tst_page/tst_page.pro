include(../test-include.pri)

QT += gui
SOURCES += tst_page.cpp

OTHER_FILES += $$system(ls tst_*.qml)
