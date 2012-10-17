import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    title: _("My custom page")
    Rectangle {
        anchors.fill: parent
        color: "#dddddd"
        TextCustom {
            anchors.centerIn: parent
            text: _("This is an external page.")
            color: "#757373"
        }
    }
}
