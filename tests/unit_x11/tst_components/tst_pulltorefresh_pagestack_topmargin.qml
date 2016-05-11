import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3
import QtTest 1.0
import Ubuntu.Test 1.0

Item {
    width: units.gu(40)
    height: units.gu(71)

    MainView {
        anchors.fill: parent
        PageStack {
            id: pageStack
            Page {
                id: page0
                title: "One"
                visible: false

                ListView {
                    id: view
                    anchors.fill: parent
                    model: 15
                    delegate: Standard {
                        width: ListView.view.width
                        height: units.gu(5)
                        text: index
                    }
                    PullToRefresh {
                        id: pullMe
                        onRefresh: {
                            refreshing = true;
                            refreshing = false;
                            pageStack.push(page1)
                        }
                    }
                }
            }
            Page {
                id: page1
                title: "Two"
                visible: false
                Button {
                    anchors.centerIn: parent
                    onClicked: pageStack.pop()
                    text: "back"
                }
            }
            Component.onCompleted: pageStack.push(page0)
        }
    }

    UbuntuTestCase {
        name: "PullToRefreshPageStackPushWhileRefreshing"
        when: windowShown

        function test_push_while_refreshing_bug1578619() {
            var initialTopMargin = view.topMargin;
            var initialContentY = view.contentY;
            flick(view, view.width/2, units.gu(10), 0, units.gu(40));
            wait(1000);
            pageStack.pop();
            tryCompare(view, "moving", false);
            tryCompare(view, "topMargin", initialTopMargin, 500,
                    "Initial topMargin of flickable is not restored after refreshing.");
            tryCompare(view, "contentY", initialContentY, 500,
                    "Initial contentY of flickable is not restored after refreshing.")
        }
    }
}
