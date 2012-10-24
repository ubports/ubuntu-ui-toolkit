import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    title: i18n.tr("My custom page")
    Rectangle {
        anchors.fill: parent
        color: "#dddddd"
        TextCustom {
            anchors.centerIn: parent
            text: i18n.tr("This is an external page.")
            color: "#757373"
        }
    }
}
