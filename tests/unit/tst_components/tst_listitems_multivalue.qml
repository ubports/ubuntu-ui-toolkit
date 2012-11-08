import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsMultiValueAPI"

     function test_text() {
         compare(listItemMultiValue.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemMultiValue.text = newText
         compare(listItemMultiValue.text,newText,"set/get")
     }

     function test_values() {
         compare(listItemMultiValue.values,undefined,"values is undefined by default")
         var newValues = ["value1","value2","value3"]
         listItemMultiValue.values = newValues
         compare(listItemMultiValue.values,newValues,"set/get")
     }

     ListItem.MultiValue {
         id: listItemMultiValue
     }
}
