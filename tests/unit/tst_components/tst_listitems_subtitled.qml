import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsSubtitledAPI"

     function test_text() {
         compare(listItemSubtitled.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemSubtitled.text = newText
         compare(listItemSubtitled.text,newText,"set/get")
     }

     function test_subText() {
         compare(listItemSubtitled.subText,"","subText is '' by default")
         var newText = "Hello World!"
         listItemSubtitled.subText = newText
         compare(listItemSubtitled.subText,newText,"set/get")
     }

     ListItem.Subtitled {
         id: listItemSubtitled
     }
}
