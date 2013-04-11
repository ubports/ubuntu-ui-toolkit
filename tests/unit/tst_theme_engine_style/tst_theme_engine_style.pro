include(../test-include.pri)
SOURCES += tst_theme_engine_styletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    TestDocument.qml \
    BindingTest.qml \
    label.qmltheme \
    LabelTest.qml
