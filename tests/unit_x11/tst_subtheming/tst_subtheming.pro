include(../test-include.pri)
QT += core-private qml-private quick-private gui-private
SOURCES += tst_subtheming.cpp

OTHER_FILES += \
    TestStyle.qml \
    SimpleItem.qml \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/Palette.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/TestStyle.qml \
    themes/TestModule/TestTheme/qmldir \
    themes/TestModule/TestTheme/parent_theme \
    DynamicAssignment.qml \
    ParentChanges.qml \
    TestMain.qml \
    TestStyleChange.qml \
    DifferentThemes.qml \
    SameNamedPaletteSettings.qml \
    ChangePaletteValueWhenParentChanges.qml \
    ChangeDefaultPaletteInChildren.qml \
    MultiplePaletteInstances.qml \
    DynamicPalette.qml \
    InvalidPalette.qml


