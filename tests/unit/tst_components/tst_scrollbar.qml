import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "ScrollbarAPI"

    function test_a()
    {
        compare(scrollbar.flickableItem, null, "Scrollbar does not have flickableItem set by default")
        var defaultAlign = Qt.AlignRight
        compare(scrollbar.align, defaultAlign, "Scrollbar is aligned to the right by default")
    }

    function test_flickableItem_Flickable()
    {
        scrollbar.flickableItem = flickable
        compare(scrollbar.flickableItem, flickable, "Cannot get/set flickableItem property")
    }

    function test_align_Flickable()
    {
        //scrollbar.flickableItem = flickable
        compare(scrollbar.flickableItem, null, "Scrollbar does not have flickableItem set by default")
        scrollbar.align = Qt.AlignLeft
        compare(scrollbar.left, flickable.left, "Cannot get/set flickableItem property")
    }

    function test_flickableItem_ListView()
    {
        scrollbar.flickableItem = listView
        compare(scrollbar.flickableItem, listView, "Cannot get/set flickableItem property")
    }

    Flickable{
        id: flickable
        width: 100
        height: 100
    }

    ListView {
        id: listView
    }

    Scrollbar {
        id: scrollbar
    }
}
