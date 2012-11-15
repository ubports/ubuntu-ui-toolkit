import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

TestCase {
    name: "PopupsComposerSheetAPI"

    function test_show() {
        composerSheet.show()
    }

    function test_hide() {
        composerSheet.hide()
    }

    function test_title() {
        compare(composerSheet.title,"","title is not set by default")

        var newTitle = "Hello World!"
        composerSheet.title = newTitle
        compare(composerSheet.title,newTitle,"set/get")
    }

    function test_contentsWidth() {
        compare(composerSheet.contentsWidth,units.gu(50),"contentsWidth is 50 grid units by default")
    }

    function test_contentsHeight() {
        compare(composerSheet.contentsHeight,units.gu(40),"contentsHeight is 40 grid units by default")
    }

    ComposerSheet {
        id: composerSheet
    }
}
