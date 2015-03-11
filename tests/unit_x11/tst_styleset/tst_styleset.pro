include(../test-include.pri)
QT += core-private qml-private quick-private gui-private
SOURCES += tst_styleset.cpp

OTHER_FILES += \
    TestStyle.qml \
    SimpleItem.qml \
    themes/AppTheme/Palette.qml \
    themes/AppTheme/parent_theme \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/TestStyle.qml \
    themes/TestModule/TestTheme/qmldir \
    themes/TestModule/TestTheme/parent_theme \
    DynamicAssignment.qml \
    ParentChanges.qml \
    TestMain.qml \
    TestStyleChange.qml \
    DifferentStylesets.qml

