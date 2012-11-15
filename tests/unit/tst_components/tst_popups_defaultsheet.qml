import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

TestCase {
    name: "PopupsDefaultSheetAPI"

    function test_show() {
        defaultSheet.show()
    }

    function test_hide() {
        defaultSheet.hide()
    }

    function test_title() {
        compare(defaultSheet.title,"","title is not set by default")

        var newTitle = "Hello World!"
        defaultSheet.title = newTitle
        compare(defaultSheet.title,newTitle,"set/get")
    }

    function test_contentsWidth() {
        compare(defaultSheet.contentsWidth,units.gu(50),"contentsWidth is 50 grid units by default")
    }

    function test_contentsHeight() {
        compare(defaultSheet.contentsHeight,units.gu(40),"contentsHeight is 40 grid units by default")
    }

    function test_doneButton() {
        compare(defaultSheet.doneButton,false,"doneButton is set to false by default")

        var newValue = true
        defaultSheet.doneButton = newValue
        compare(defaultSheet.doneButton,newValue,"set/get")
    }

    DefaultSheet {
        id: defaultSheet
    }
}
