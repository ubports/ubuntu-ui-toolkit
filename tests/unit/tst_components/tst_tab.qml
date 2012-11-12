import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "TabAPI"

     function test_iconSource() {
        compare(tab.iconSource,"","iconSource is not set by default")
        var newIcon = "../../../demos/small_avatar.png"
        tab.iconSource = newIcon
        compare(tab.iconSource,Qt.resolvedUrl(newIcon),"Can set/get iconSource")
     }

     function test_page() {
        compare(tab.page,undefined,"page is undefined by default")
     }

     function test_title() {
         compare(tab.title,"","is not set by default")
         var newTitle = "Hello World!"
         tab.title = newTitle
         compare(tab.title,newTitle,"can set/get")
     }


     Tab {
         id: tab
     }
}
