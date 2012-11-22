import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

TestCase {
    name: "PopupsPopoverAPI"

    function test_show() {
        popOver.show()
    }

    function test_hide() {
        popOver.hide()
    }

    function test_caller() {
        compare(popOver.caller,null,"caller is not set by default")
    }

    Popover {
        id: popOver
        Text {
            text: "Hello Popover!"
        }
    }
}
