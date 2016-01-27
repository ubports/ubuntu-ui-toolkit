include(../test-include.pri)
QT += core-private qml-private quick-private gui-private
SOURCES += tst_subtheming.cpp

OTHER_FILES += \
    TestStyle.qml \
    SimpleItem.qml \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/Palette.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/1.2/TestStyle.qml \
    themes/TestModule/TestTheme/1.3/TestStyle.qml \
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
    InvalidPalette.qml \
    ReparentStyledItemFollowsNewPathOnly.qml \
    themes/TestModule/TestTheme/1.3/TestStyle.qml \
    themes/CustomTheme/1.3/TestStyle.qml \
    StyledItemV12.qml \
    StyledItemV13.qml \
    DeprecatedTheme.qml \
    StyledItemAppThemeVersioned.qml \
    StyleOverride.qml \
    StyleKept.qml \
    SimplePropertyHints.qml \
    StyleHintsWithSignal.qml \
    StyleHintsWithObject.qml \
    StyleHintsElsewhere.qml \
    StyleHintsInvalidProperty.qml \
    PropertyBindingHints.qml \
    MoreStyleHints.qml \
    GroupPropertyBindingHints.qml \
    OverrideStyleHints.qml \
    HintedButton.qml \
    OtherVersion.qml

DISTFILES += \
    DefaultTheme.qml


