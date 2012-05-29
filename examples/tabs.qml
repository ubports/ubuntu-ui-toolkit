import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    Toolbar {
        id: toolbar
        align: "top"
    }

    Tabs {
        toolbar: toolbar
        align: "center"

        TabPage {
            tabButton.text: "first tab" // text on the tabbutton
            TextCustom { anchors.centerIn: parent; text: "page content" }
        }
        TabPage {
            tabButton.text: "second tab"
            tabButton.color: "green"
            tabButton.activeColor: "yellow"

            TextCustom { anchors.centerIn: parent; text: "empty page" }
        }
        TabPage {
            tabButton { name: "third"; color: "orange"; activeColor: "blue"; activeHeight: 50 }

            // whatever can be on a page
        }

        TabButton { text: "first tab" }
        TabPage {
            Button { anchors.centerIn: parent; width: 200; height: 100; text: "just a button" }
        } // TabPage
        TabButton { text: "second tab" }
        TabPage {
            TextCustom { anchors.centerIn: parent; text: "empty page" }
        } // TabPage
        TabButton { text: "third!" }
        TabPage {
            TextCustom { anchors.centerIn: parent; text: "and another page" }
        } // TabPage
    } // Tabs
} // window
