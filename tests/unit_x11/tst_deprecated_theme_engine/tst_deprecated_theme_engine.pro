include(../test-include.pri)
QT += quick-private gui-private
SOURCES += tst_deprecated_theme_engine.cpp

OTHER_FILES += \
    TestStyle.qml \
    TestApp.qml \
    Parent.qml \
    SimpleItem.qml \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/TestStyle.qml \
    themes/TestModule/TestTheme/qmldir \
    themes/TestModule/TestTheme/parent_theme
