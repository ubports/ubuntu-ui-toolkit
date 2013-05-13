import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(50)
    height: units.gu(80)

    Action {
        id: action1
        text: "action 1"
        onTriggered: print("one!")
    }
    Action {
        id: action2
        text: "action 2"
        onTriggered: print("two!")
    }


    Page {
        title: "test page"

        Label {
            anchors.centerIn: parent
            text: "Hello, world"
        }

        tools: ToolbarActions {
            actions: [action1, action2]

//            Action {
//                id: action3
//                text: "action 3"
//                onTriggered: print("three!")
//            }
        }
    }

}
