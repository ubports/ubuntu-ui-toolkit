import QtQuick 2.4
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1

Item {
    id: main
    width: units.gu(100)
    height: units.gu(71)

    Page {
        id: page1
        title: "Page #1"

        Layout.maximumWidth: units.gu(40)
    }

    Page {
        id: page2
        title: "Page #2"
    }
    Page {
        id: page3
        title: "Page #3"
    }
    Page {
        id: page4
        title: "Page #4"
    }

    Rectangle {
        id: rect
        color: "pink"
        width: units.gu(30)
        height: units.gu(50)
    }

    MultiColumnView {
        anchors.fill: parent
        columns: width > units.gu(80) ? 2 : 1
        Component.onCompleted: addToNextColumn(null, rect)
    }

}
