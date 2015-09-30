import QtQuick 2.0
import Ubuntu.Components 1.0
import Ubuntu.Components.Popups 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Button {
        objectName: "open_popover"
        text: "Open Popover"
        onClicked: testActionsPopover.show();
    }

    Label {
        id: "label"
        objectName: "clicked_label"
        anchors.centerIn: parent
        text: "Button not clicked."
    }

    ActionSelectionPopover {
        objectName: "test_actions_popover"
        id: testActionsPopover
        actions: ActionList {
            Action {
                text: "Action one"
                objectName: "actionOne"
                onTriggered: label.text = "Button clicked."
            }
            Action {
                text: "Action two"
                objectName: "actionDisabled"
                onTriggered: label.text = "Disabled button clicked."
            }
            Action {
                text: "Action three"
                objectName: "actionHidden"
                onTriggered: label.text = "Hidden button clicked."
            }
        }
    }
}
