import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    title: gallery.tr("My custom page")
    Rectangle {
        anchors.fill: parent
        color: "#dddddd"
        TextCustom {
            anchors.centerIn: parent
            text: gallery.tr("This is an external page.")
            color: "#757373"
        }
    }
}
