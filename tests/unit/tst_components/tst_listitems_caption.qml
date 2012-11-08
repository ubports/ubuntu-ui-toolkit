import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsCaptionAPI"

     function test_text() {
         compare(listItemCaption.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemCaption.text = newText
         compare(listItemCaption.text,newText,"set/get")
     }

     ListItem.Caption {
         id: listItemCaption
     }
}
