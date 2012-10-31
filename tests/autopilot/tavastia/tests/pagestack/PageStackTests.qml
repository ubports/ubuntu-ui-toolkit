import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

PageStack {
    id: pageStack

    width: 300
    height: 400

    Component.onCompleted: push(page0)

    Page {
        id: page0
        title: "Root page"

        Column {
            anchors.fill: parent
            ListItem.Standard {
                text: "Page one"
                onClicked: pageStack.push(page1)
                progression: true
            }
        }
    }

    Page {
        id: page1
        title: "First page"

        Column {
            anchors.fill: parent
          
            ListItem.Standard {
                text: "Red rectangle"
                onClicked: pageStack.push(rect, {color: "red"})
                progression: true
            }
        }
    }

    Component {
        id: rect
        Rectangle {
            id: rectangle
            anchors.fill: parent
            color: "green"
        }
    }
}
