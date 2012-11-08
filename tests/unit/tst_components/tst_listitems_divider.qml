import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

TestCase {
     name: "ListItemsDividerAPI"

     function test_divider() {
         verify((listItemDivider),"Divider can be loaded")
     }

     ListItem.Divider {
         id: listItemDivider
     }
}
