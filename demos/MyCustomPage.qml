import QtQuick 1.1
import Ubuntu.Components 0.1

Page {
    title: "Loaded page"
    anchors.fill: parent ? parent : undefined
    Rectangle {
        color: "#dddddd"
        anchors.fill: parent
        TextCustom {
            anchors.centerIn: parent
            text: "This is an external page."
            color: "#757373"
        }
    }

}
