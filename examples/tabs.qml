import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    Row {
        id: buttonRow
        anchors.horizontalCenter: window.horizontalCenter
        TabButton {
            text: "Tab 1"
            tab: tab1
        }
        TabButton {
            text: "Tab 2"
            tab: tab2
        }
        TabButton {
            text: "Tab 3"
            tab: tab3
        }
    } // Row

   TabGroup {
       currentTab: tab1
       anchors.top: buttonRow.bottom
       anchors.horizontalCenter: window.horizontalCenter

       Rectangle {
           anchors.horizontalCenter: parent.horizontalCenter
           id: tab1
           width: 50; height: 100
           color: "green"
       }

       Rectangle {
           id: tab2
           width: 50; height: 100
           color: "black"
       }

       Rectangle {
           id: tab3
           width: 50; height: 100
           color: "orange"
       }
   }

} // window
