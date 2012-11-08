import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsSingleValueAPI"

     function test_text() {
         compare(listItemSingleValue.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemSingleValue.text = newText
         compare(listItemSingleValue.text,newText,"set/get")
     }

     function test_value() {
         compare(listItemSingleValue.value,"","value is '' by default")
         var newValue = "New Value"
         listItemSingleValue.value = newValue
         compare(listItemSingleValue.value,newValue,"set/get")
     }

     ListItem.SingleValue {
         id: listItemSingleValue
     }
}
