include(../test-include-x11.pri)

QT += gui
SOURCES += tst_page.cpp

OTHER_FILES += $$system(ls tst_*.qml)
