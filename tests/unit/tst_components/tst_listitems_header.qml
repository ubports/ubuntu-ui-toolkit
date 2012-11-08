import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsHeaderAPI"

     function test_text() {
         compare(listItemHeader.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemHeader.text = newText
         compare(listItemHeader.text,newText,"set/get")
     }

     ListItem.Header {
         id: listItemHeader
     }
}
