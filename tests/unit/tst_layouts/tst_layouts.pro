include(../test-include.pri)

QT += gui
SOURCES += \
    tst_layouts.cpp

OTHER_FILES += \
    SimpleLayouts.qml \
    NoLayouts.qml \
    OverlappingCondition.qml \
    SmallLayout.qml \
    MediumLayout.qml \
    LargeLayout.qml \
    ExternalLayouts.qml \
    NestedLayouts.qml \
    DefaultLayout.qml \
    ResizingContainers.qml \
    SizedDefaultLayout.qml \
    CurrentLayoutChange.qml \
    PositioningOnLayoutChange.qml
