import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsSingleControlAPI"

     function test_control() {
         compare(listItemSingleControl.control,null,"control is null by default")
         listItemSingleControl.control = testControl
         compare(listItemSingleControl.control,testControl,"set/get")
     }

     AbstractButton {
         id: testControl
     }

     ListItem.SingleControl {
         id: listItemSingleControl
     }
}
