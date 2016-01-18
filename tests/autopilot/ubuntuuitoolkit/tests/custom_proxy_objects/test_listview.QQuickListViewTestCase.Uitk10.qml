import QtQuick 2.0
import Ubuntu.Components 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(20)
    objectName: "mainView"

    Page {

        Column {
            id: column
            width: units.gu(48)
            height: units.gu(20)

            Label {
                id: clickedLabel
                objectName: "clickedLabel"
                text: "No element clicked."
            }

            ListView {
                id: testListView
                objectName: "testListView"
                anchors.left: parent.left
                anchors.right: parent.right
                height: column.height - clickedLabel.paintedHeight
                clip: true
                model: 20

                delegate: ListItem.Standard {
                    objectName: "testListElement%1".arg(index)
                    text: "test list element %1".arg(index)
                    onClicked: clickedLabel.text = objectName
                    height: units.gu(5)
                }
            }
        }
    }
}
