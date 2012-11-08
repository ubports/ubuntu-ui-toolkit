import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsBaseAPI"

     function test_fallbackIconSource() {
         compare(listItemBase.fallbackIconSource,undefined,"fallbackIconSource is not set by default")
     }

     function test_icon() {
         compare(listItemBase.icon,undefined,"icon is not set by default")

         // test with item
         listItemBase.icon = testItem
         compare(listItemBase.icon,testItem,"set/get from Item")

         // test with url
         var newIcon = "../../../demos/small_avatar.png"
         listItemBase.icon = newIcon
         compare(listItemBase.icon,newIcon,"set/get from url")
     }

     function test_iconFrame() {
         compare(listItemBase.iconFrame,true,"iconFrame is true by default")
     }

     function test_progression() {
         compare(listItemBase.progression,false,"progression is false by default")
     }

     Rectangle {
         id: testItem
     }

     ListItem.Base {
         id: listItemBase
     }
}
