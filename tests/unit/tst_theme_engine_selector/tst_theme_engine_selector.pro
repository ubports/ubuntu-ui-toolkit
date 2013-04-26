include(../test-include.pri)
SOURCES += tst_theme_engine_selectortest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    TestTheme.qmltheme \
    SimpleSelector.qml \
    DirectChildSelector.qml \
    DescendantSelector.qml \
    SimpleSelectorWithName.qml \
    DirectChildSelectorWithName.qml \
    DirectChildSelectorWithNamedParent.qml \
    DescendantSelectorWithName.qml \
    DescendantSelectorWithNamedParent.qml
