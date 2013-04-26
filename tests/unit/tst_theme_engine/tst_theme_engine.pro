include(../test-include.pri)
QT += gui
SOURCES += \
    tst_theme_enginetest.cpp
RESOURCES += \
    ../../resources/theme-engine-test.qrc

OTHER_FILES += ReparentingTest.qml \
    SelectorTest.qml \
    InheritanceTest.qml \
    MemoryCleanup.qml
