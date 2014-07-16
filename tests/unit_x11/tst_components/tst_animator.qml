/**
	Test to prevent regressions for bug: https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1338602
	Activity Indicator crashes in QML/Widget mixed applications
*/

import QtQuick 2.2
import QtQuick.Window 2.1
import QtTest 1.0
import Ubuntu.Components 1.1

Item {
    width: 360
    height: 360
    Window {
        id: root
        width: 360
        height: 360

        property int timeout: 500
        property int runs: 20
        property bool finished: false

        TestCase {
            name: "AnimatorRegressionTest"
            when: windowShown

            function test_show_hide () {
                tryCompare(root,"finished",true,root.timeout * (root.runs+1));
            }
        }

        Timer {
            property int hits: 0
            interval: root.timeout; running: !root.finished; repeat: !root.finished
            onTriggered: {
                hits++;
                if (hits >= root.runs) {
                    root.visible = true;
                    root.finished = true;
                    return;
                }

                root.visible = !root.visible
            }
        }

        Flow {
            anchors.fill: parent
            Repeater {
                model: 3

                ActivityIndicator {
                    running: true
                }
            }
        }
    }
}
