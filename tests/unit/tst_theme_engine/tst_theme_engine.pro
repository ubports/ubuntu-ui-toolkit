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
    NonStyledRepeaterParent.qml \
    NonStyledListViewParent.qml \
    ReParentNonStyledToNonStyled.qml \
    ReParentNonStyledToStyled.qml \
    ReParentNonStyledToStyledTwice.qml \
    Reparenting1.qmltheme \
    Reparenting2.qmltheme \
    ReParentNonStyledToNonStyledTwice.qml \
    MemoryCleanup.qml
