include(../test-include-x11.pri)
QT += core-private qml-private quick-private gui-private UbuntuToolkit

SOURCES += \
    tst_asyncloader.cpp

DISTFILES += \
    Document.qml \
    TestApp.qml \
    HeavyDocument.qml \
    FaultyDocument.qml
