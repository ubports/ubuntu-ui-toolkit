import QtQuick 1.1
import Ubuntu.Components 0.1

Page {
    title: "Custom page title"
    contents:  Rectangle {
        color: "#dddddd"
        anchors.fill: parent ? parent : undefined
        TextCustom {
            anchors.centerIn: parent
            text: "This is an external page."
            color: "#757373"
        }
    }
}
