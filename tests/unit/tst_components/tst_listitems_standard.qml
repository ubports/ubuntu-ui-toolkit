import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsStandardAPI"

     function test_fallbackIconSource() {
         skip("https://bugs.launchpad.net/tavastia/+bug/1076762")
         compare(listItemStandard.fallbackIconSource,undefined,"fallbackIconSource is not set by default")
     }

     function test_control() {
         compare(listItemStandard.control,null,"control is null by default")
         listItemStandard.control = testControl
         compare(listItemStandard.control,testControl,"set/get")
     }

     function test_icon() {
         compare(listItemStandard.icon,undefined,"icon is not set by default")

         // test with item
         listItemStandard.icon = testItem
         compare(listItemStandard.icon,testItem,"set/get from Item")

         // test with url
         var newIcon = "../../../demos/small_avatar.png"
         listItemStandard.icon = newIcon
         compare(listItemStandard.icon,newIcon,"set/get from url")
     }

     function test_iconFrame() {
         compare(listItemStandard.iconFrame,true,"iconFrame is true by default")
     }

     function test_progression() {
         compare(listItemStandard.progression,false,"progression is false by default")
     }

     function test_text() {
         compare(listItemStandard.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemStandard.text = newText
         compare(listItemStandard.text,newText,"set/get")
     }

     Rectangle {
         id: testItem
     }

     AbstractButton {
         id: testControl
     }

     ListItem.Standard {
         id: listItemStandard
     }
}
