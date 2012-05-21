import QtQuick 1.1

ColoredButton {
    id: textButton
    property alias text: label.text
    property alias textColor: label.color
    property alias textSize: label.fontSize

    TextCustom {
        id: label
        anchors.centerIn: parent
        fontSize: "medium"
    } // label
} // button
