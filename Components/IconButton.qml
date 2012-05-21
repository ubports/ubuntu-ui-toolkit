import QtQuick 1.1

ColoredButton {
    property alias icon: icon.source

    property int verticalMargin: 1;

    Image {
        anchors.centerIn: parent
        id: icon
        height: parent.height-(2*verticalMargin)
        fillMode: Image.PreserveAspectFit
    }
}

