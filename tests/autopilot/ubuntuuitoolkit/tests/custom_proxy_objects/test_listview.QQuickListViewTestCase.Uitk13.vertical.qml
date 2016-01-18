import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(20)
    objectName: "mainView"

    Page {
        header: Item { }

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
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: column.height - clickedLabel.paintedHeight
                clip: true
                model: 20

                delegate: ListItem {
                    objectName: "testListElement%1".arg(index)
                    Label {
                        anchors.centerIn: parent
                        text: "test list element %1".arg(index)
                    }
                    onClicked: clickedLabel.text = objectName
                    height: units.gu(5)
                }
            }
        }
    }
}
