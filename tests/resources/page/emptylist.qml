import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: 300
    height: 450

    PageStack {
        id: pageStack
        Component.onCompleted: push(soundsPage)

        Page {
            id: soundsPage

            Column {
                id: columnId
                anchors.left: parent.left
                anchors.right: parent.right

                ListItem.SingleControl {
                    id: listId
                    control: Button {
                        text: i18n.tr("Stop playing")
                        width: parent.width - units.gu(4)
                    }
                }
            }

            Flickable {
                width: parent.width
                contentHeight: contentItem.childrenRect.height
                anchors.top: columnId.bottom
                anchors.bottom: soundsPage.bottom
                clip: true

                ListItem.ItemSelector {
                    id: soundSelector
                    expanded: true
                    model: ["some", "items", "to", "list"]
                    onDelegateClicked: {
                        print("clicked")
                    }
                }
            }
        }
    }
}

