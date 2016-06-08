include(../test-include-x11.pri)
QT += quick-private gui-private
SOURCES += tst_inversemouseareatest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    InverseMouseAreaPropagateEvents.qml \
    InverseMouseAreaDoNotPropagateEvents.qml \
    SensingAreaError.qml \
    InverseMouseAreaInWindow.qml \
    OverlappedMouseArea.qml \
    InverseMouseAreaOnTop.qml \
    InverseMouseAreaSignals.qml \
    InverseMouseAreaNormalEventStack.qml \
    InverseMouseAreaTopmostItem.qml \
    InverseMouseAreaSensingArea.qml \
    InverseMouseAreaOnTopNoAccept.qml \
    Defaults.qml \
    InverseMouseAreaInListView.qml \
    InverseMouseAreaInPage.qml \
    InverseMouseAreaInFlickable.qml \
    InverseMouseAreaParentClipped.qml \
    InverseMouseAreaClip.qml
