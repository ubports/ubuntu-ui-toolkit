include(../test-include.pri)
SOURCES += tst_performance.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    UbuntuShapeGrid.qml \
    ButtonDelegateGrid.qml \
    PairOfUbuntuShapeGrid.qml \
    ButtonGrid.qml \
    CheckBoxDelegateGrid.qml \
    CheckBoxGrid.qml \
    SwitchDelegateGrid.qml \
    SwitchGrid.qml \
    LabelGrid.qml \
    SliderDelegateGrid.qml \
    SliderGrid.qml \
    TextGrid.qml \
    RectangleGrid.qml \
    CustomTheme.qmltheme \
    ButtonsWithStyledGrid.qml \
    TextWithImport.qml \
    TextWithImportGrid.qml
