include(../test-include.pri)
QT += quick-private gui-private
SOURCES += tst_multi_theming.cpp

OTHER_FILES += \
    TestStyle.qml \
    SimpleItem.qml \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/TestStyle.qml \
    themes/TestModule/TestTheme/qmldir \
    themes/TestModule/TestTheme/parent_theme
