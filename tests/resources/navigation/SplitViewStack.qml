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
        anchors.fill: parent
        objectName: "PinkPage"
    }

    Rectangle {
        id: rect2
        color: "green"
        anchors.fill: parent
        objectName: "GreenPage"
    }

    Rectangle {
        id: rect3
        color: "blue"
        anchors.fill: parent
        objectName: "BluePage"
    }

    MultiColumnView {
        id: view
        anchors.fill: parent
        columns: {
            if (width > units.gu(120)) return 3;
            if (width > units.gu(80)) return 2;
            return 1;
        }
        Component.onCompleted: {
            addToNextColumn(null, rect)
            addToNextColumn(rect, rect2);
            addToCurrentColumn(rect2, rect3);
        }
    }

}
