import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsEmptyAPI"

     function test_highlightWhenPressed() {
         compare(listItemEmpty.highlightWhenPressed,true,"highlightWhenPressed is true by default")
         listItemEmpty.highlightWhenPressed = false
         compare(listItemEmpty.highlightWhenPressed,false,"set/get")
     }

     function test_hovered() {
        compare(listItemEmpty.hovered,false,"hovered is false by default")
     }

     function test_pressed() {
        compare(listItemEmpty.pressed,false,"pressed is false by default")
     }

     function test_selected() {
        compare(listItemEmpty.selected,false,"selected is false by default")
     }

     function test_showDivider() {
        compare(listItemEmpty.showDivider,true,"showDivider is true by default")
        listItemEmpty.showDivider = false
        compare(listItemEmpty.showDivider,false,"set/get")
     }

     ListItem.Empty {
         id: listItemEmpty
     }
}
