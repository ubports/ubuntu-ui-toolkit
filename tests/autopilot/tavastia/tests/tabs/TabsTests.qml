    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Tabs {
       id: tabs
       Tab {
           id: tab1
           title: "Tab 1"
           page: Rectangle {
              anchors.fill: parent 
              color: "#eeeeee"
           }
       }

       Tab {
           id: tab2
           title: "Tab 2"
           page: Rectangle {
              anchors.fill: parent  
              color: "#eeeeee"
           }
       }
    }
