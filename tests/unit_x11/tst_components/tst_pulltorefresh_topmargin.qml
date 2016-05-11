import QtQuick 2.2
import Ubuntu.Components 1.3
import QtTest 1.0
import Ubuntu.Test 1.0

Item {
    width: units.gu(40)
    height: units.gu(71)
    id: root
    Flickable {
        id: view
        anchors.fill: parent
        contentHeight: 1.6*view.height
        Label {
            anchors.centerIn: parent
            text: "void"
        }
        PullToRefresh {
            id: pullToRefresh
            parent: view
            onRefresh: {
                refreshing = true;
                refreshing = false;
            }
        }
    }

    UbuntuTestCase {
        name: "PullToRefreshTopMargin"
        when: windowShown

        function test_reposition_after_refresh_bug1578619() {
            var initialTopMargin = view.topMargin;
            var initialContentY = view.contentY;
            flick(view, view.width/2, units.gu(10), 0, units.gu(40));
            tryCompare(view, "moving", false);
            tryCompare(view, "topMargin", initialTopMargin, 500,
                    "Initial topMargin of flickable is not restored after refreshing.");
            tryCompare(view, "contentY", initialContentY, 500,
                    "Initial contentY of flickable is not restored after refreshing.")
        }
    }
}
