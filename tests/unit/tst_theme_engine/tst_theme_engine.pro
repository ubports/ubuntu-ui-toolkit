include(../test-include.pri)
QT += gui
SOURCES += \
    tst_theme_enginetest.cpp
RESOURCES += \
    ../../resources/theme-engine-test.qrc

OTHER_FILES += ReparentingTest.qml \
    SelectorTest.qml \
    InheritanceTest.qml \
    NonStyledParent.qml \
    NonStyledParent.qmltheme \
    NonStyledRepeaterParent.qml \
    NonStyledListViewParent.qml \
    ReParentNonStyledToNonStyled.qml \
    ReParentNonStyledToStyled.qml
