include(../test-include-x11.pri)
include(../../unit/qtprivate_dependency.pri)

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
    PositioningOnLayoutChange.qml \
    LaidOutItemsOutsideOfLayout.qml \
    OverlaidInItemLayout.qml \
    AnchorFilledReparenting.qml \
    AnchorCenteredInDefault.qml \
    AnchorFilledMargins.qml \
    AnchorFilledSeparateMargins.qml \
    AnchorVerticalCenter.qml \
    AnchorVerticalCenterOffset.qml \
    AnchorCenterWithOffset.qml \
    AnchorHorizontalCenter.qml \
    AnchorHorizontalCenterOffset.qml \
    AnchorLeft.qml \
    AnchorRight.qml \
    AnchorTop.qml \
    AnchorBottom.qml \
    AnchorAll.qml \
    ItemInstanceAsProperty.qml \
    DialerCrash.qml \
    ExcludedItemDeleted.qml \
    Visibility.qml \
    NestedVisibility.qml
