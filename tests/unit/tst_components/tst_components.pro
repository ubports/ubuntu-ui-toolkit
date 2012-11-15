include(../qmltest-include.pri)

TARGET = tst_components

SOURCES += tst_components.cpp

OTHER_FILES += $$system(ls *.qml) \
    tst_popups_composersheet.qml \
    tst_popups_defaultsheet.qml \
    tst_popups_dialog.qml
