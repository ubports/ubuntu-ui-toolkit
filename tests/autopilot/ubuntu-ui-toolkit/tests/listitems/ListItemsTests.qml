import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    width: 350
    height: 500
    Column {
        anchors.fill: parent

        ListItem.Header { 
            id: listItemHeader
            text: "Header" 
        }
        ListItem.Standard {
            id: listItemStandard
            text: "Standard"
        }
        ListItem.SingleValue {
            id: listItemSingleValue
            text: "Single Value"
            value: "Value"
        }
        ListItem.MultiValue {
	    id: listItemMultiValue
            text: "Multi Value"
            values: ["Value 1", "Value 2", "Value 3", "Value 4"]
        }

        ListItem.Subtitled {
            id: listItemSubtitled
            text: "Subtitled"
            subText: "Secondary label"
        }

        ListItem.Caption {
            id: listItemCaption
            text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
        }

        ListItem.Divider {
            id: listItemDivider
        }

        ListItem.ValueSelector {
            id: listItemValueSelector
            text: "Value Selector"
            values: ["Value 1", "Value 2", "Value 3", "Value 4"]
        }

        ListItem.Standard {
            id: listItemWithControl
            text: "With Control"
            control: Switch {
                anchors {
                   top: parent.top
               bottom: parent.bottom
                }
            }
        }
    }
}
