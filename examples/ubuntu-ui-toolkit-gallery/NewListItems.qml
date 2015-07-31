/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import Ubuntu.Components 1.3

Template {
    objectName: "listItemsTemplate"

    TemplateSection {
        className: "ListItemLayoutTemplates"
        // no spacing between the list items in the Column
        spacing: 0
        /*GenericListItemLayout {
            firstItem: CheckBox { id: box; }
            thirdItem: Icon { width: units.gu(2); name: "email" }
            fourthItem: Icon { width: units.gu(2); name: "edit" }
            firstSlot.onClicked: box.checked = !box.checked
            mainInputArea.onClicked: console.log("PRIMARY ACTION")

        }

        ListItem {
            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            onClicked: console.log("LISTITEM CLICKED")

            GenericListItemLayout {
                firstItem: CheckBox { id: box1  }
                thirdItem: Icon { id: icon1; width: units.gu(2); name: "email" }
                fourthItem: Icon { width: units.gu(2); name: "edit" }
                firstSlot.onClicked: box1.checked = !box1.checked
                thirdSlot.onClicked: console.log("THIRDSLOT TRIGGERED")
                mainInputArea.onClicked: console.log("PRIMARY ACTION")
            }
        }*/

        /*ListItem {
            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            HardcodedFourContainersLayout {
                anchors.fill: parent
                //the dev has to specify the width because the container will use that to
                //resize itself
                firstSlot: CheckBox { }
                firstSlotArea.onClicked: doSomething()

                secondSlot: Icon { width: units.gu(2); name: "email" }
                secondSlotArea.onClicked: doSomethingElse()

                thirdSlot: Icon { width: units.gu(2); name: "message" }
                thirdSlotArea.onClicked: doSomethingAwesome()

                mainArea.onClicked: console.log("PRIMARY ACTION")

                //set this if your slots don't have actions associated
                firstSlotArea.enabled: false
                secondSlotArea.enabled: false
                thirdSlotArea.enabled: false
            }
        }*/

        /*ListItem {

            height: row.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            Row {
                id: row
                width: parent.width
                //use the max height of the components + 2gu margin per side
                height: Math.max(labelsContainer.height, Math.max(Math.max(item1.height, item2.height), item3.height)) + units.gu(4)
                MouseArea {
                    id: slot1
                    height: parent.height
                    width: item1.width + units.gu(3)
                    Item { id: item1; anchors.centerIn: parent; width: units.gu(2) }
                }
                Item {
                    id: labelsContainer
                    height: title.height + subtitle.height + units.gu(1)
                    width: parent.width - slot1.width - slot2.width - slot3.width
                    anchors.verticalCenter: parent.verticalCenter
                    Text { id: title; width: parent.width; text: "test"; font.weight: Font.Light; color: "#525252"; font.pixelSize: FontUtils.sizeToPixels("medium") }
                    Text { id: subtitle
                        anchors.top: title.baseline;
                        anchors.topMargin: units.gu(1);
                        width: parent.width;
                        text: "test";
                        font.weight: Font.Light;
                        color: "#525252";
                        font.pixelSize: FontUtils.sizeToPixels("small")
                    }
                }
                MouseArea {
                    id: slot2
                    height: parent.height
                    width: item2.width + units.gu(3)
                    Item { id: item2; anchors.centerIn: parent; width: units.gu(2) }
                }

                MouseArea {
                    id: slot3
                    height: parent.height
                    width: item3.width + units.gu(3)
                    Item { id: item3; anchors.centerIn: parent; width: units.gu(2) }
                }
            }
        }*/

        /*ListItem {
            id: benchmarkListItem
            height: layoutBenchmark.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            SlotsLayoutCpp {
                id: layoutBenchmark
                SlotQML { position: "Slot.Leading"; Item { width: units.gu(2) } }
                SlotQML { Item { width: units.gu(2) } }
                SlotQML { Item { width: units.gu(2) } }
                titleItem.text: "test"
                subtitleItem.text: "test"
            }
        }*/

        ListItem {
            id: listItem

            //FIXME: do we like this?
            height: layout.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            SlotsLayoutCpp {
                id: layout

                SlotQML {
                    position: "Slot.Leading"
                    //handleInnerItemPosition: false
                    CheckBox { /*y: layout.titleItem.y; anchors.horizontalCenter: parent.horizontalCenter*/ }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(2); name: "email" }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(2); name: "message" }
                }

                titleItem.text: "Hello designers!"
                subtitleItem.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
                subsubtitleItem.text: "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
            }
        }

        ListItem {
            id: listItem2

            //FIXME: do we like this?
            height: layout2.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            SlotsLayoutCpp {
                id: layout2

                SlotQML {
                    position: "Slot.Leading"
                    CheckBox { }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Rectangle { width: units.gu(10); height: units.gu(10) }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(2); name: "message" }
                }

                titleItem.text: "Hello designers!"
                //subtitleItem.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
                //subsubtitleItem.text: "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
            }
        }

        ListItem {
            id: listItem3

            //FIXME: do we like this?
            height: layout3.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            SlotsLayoutCpp {
                id: layout3

                SlotQML {
                    position: "Slot.Leading"
                    CheckBox { width: units.gu(10); height: units.gu(10) }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Rectangle { width: units.gu(5); height: units.gu(5) }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(5); height: units.gu(5); name: "message" }
                }

                titleItem.text: "Hello designers!"
                subtitleItem.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
                subsubtitleItem.text: "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
            }
        }

        ListItem {
            height: qmlLayout.height

            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            SlotsLayout {
                id: qmlLayout

                SlotQML {
                    position: "Slot.Leading"
                    //handleInnerItemPosition: false
                    CheckBox { }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(2); name: "email" }
                }

                SlotQML {
                    position: "Slot.Trailing"
                    Icon { width: units.gu(2); name: "message" }
                }

                titleItem.text: "TestTitle"
                subtitleItem.text: "TestSubtitle"
            }
        }

    }
    TemplateSection {
        className: "ListItem"
        // no spacing between the list items in the Column
        spacing: 0
        Item {
            // compensate for the spacing of 0 by adding this
            // Item inbetween the title and the list items.
            height: units.gu(3)
            width: parent.width
        }

        // clip the action delegates while swiping left/right
        clip: true

        ListItemWithLabel {
            color: UbuntuColors.blue
            text: i18n.tr("Colored")
        }
        ListItemWithLabel {
            text: i18n.tr("Highlight color")
            highlightColor: UbuntuColors.orange
            // no highlight without clicked() or leading/trailing actions
        }

        ListItemActions {
            id: exampleLeadingActions
            actions: [
                Action {
                    iconName: "tick"
                },
                Action {
                    iconName: "delete"
                }
            ]
        }
        ListItemActions {
            id: exampleTrailingActions
            actions: [
                Action {
                    iconName: "edit"
                },
                Action {
                    iconName: "save-to"
                }
            ]
        }

        ListItemWithLabel {
            text: i18n.tr("Leading actions")
            leadingActions: exampleLeadingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Trailing actions")
            trailingActions: exampleTrailingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Leading and trailing actions")
            leadingActions: exampleLeadingActions
            trailingActions: exampleTrailingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Custom action delegates")
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "tick"
                        property color color: UbuntuColors.green
                    },
                    Action {
                        iconName: "delete"
                        property color color: UbuntuColors.red
                    }
                ]
                delegate: Rectangle {
                    width: height
                    color: pressed ? Qt.darker(action.color, 1.2) : action.color
                    Icon {
                        width: units.gu(3)
                        height: width
                        name: action.iconName
                        color: "white"
                        anchors.centerIn: parent
                    }
                }
            }
            trailingActions: ListItemActions {
                actions: [
                    Action {
                        text: i18n.tr("Edit")
                    },
                    Action {
                        text: i18n.tr("Move")
                    }
                ]
                delegate: Rectangle {
                    color: pressed ? Qt.darker("yellow", 1.1) : "yellow"
                    width: units.gu(10)
                    Label {
                        text: action.text
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }

    TemplateSection {
        className: "ListItem"
        title: "Select mode"

        ListView {
            height: units.gu(20)
            width: parent.width
            clip: true

            ViewItems.dragMode: ViewItems.selectMode
            ViewItems.onDragUpdated: {
                if (event.status == ListItemDrag.Started) {
                    if (model[event.from] == "Immutable")
                        event.accept = false;
                    return;
                }
                if (model[event.to] == "Immutable") {
                    event.accept = false;
                    return;
                }
                // No instantaneous updates
                if (event.status == ListItemDrag.Moving) {
                    event.accept = false;
                    return;
                }
                if (event.status == ListItemDrag.Dropped) {
                    var fromItem = model[event.from];
                    var list = model;
                    list.splice(event.from, 1);
                    list.splice(event.to, 0, fromItem);
                    model = list;
                }
            }

            model: [ i18n.tr("Basic"), i18n.tr("Colored divider"), i18n.tr("Immutable"), i18n.tr("No divider") ]
            delegate: ListItemWithLabel {
                text: modelData
                color: dragging ? "lightblue" : "transparent"
                divider {
                    colorFrom: modelData == i18n.tr("Colored divider") ? UbuntuColors.red : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    colorTo: modelData == i18n.tr("Colored divider") ? UbuntuColors.green : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    visible: modelData != i18n.tr("No divider")
                }
            }
        }
    }

    TemplateSection {
        className: "ListItem"
        title: "Drag mode"

        UbuntuListView {
            height: units.gu(20)
            width: parent.width
            clip: true
            ViewItems.dragMode: true
            ViewItems.onDragUpdated: {
                if (event.status == ListItemDrag.Started) {
                    if (model.get(event.from).label == "Immutable")
                        event.accept = false;
                    return;
                }
                if (model.get(event.to).label == "Immutable") {
                    event.accept = false;
                    return;
                }
                // Live update as you drag
                if (event.status == ListItemDrag.Moving) {
                    model.move(event.from, event.to, 1);
                }
            }


            model: ListModel {
                ListElement { label: "Basic" }
                ListElement { label: "Colored divider" }
                ListElement { label: "Immutable" }
                ListElement { label: "No divider" }
            }

            delegate: ListItemWithLabel {
                text: modelData
                color: dragMode ? "lightblue" : "lightgray"
                divider {
                    colorFrom: modelData == i18n.tr("Colored divider") ? UbuntuColors.red : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    colorTo: modelData == i18n.tr("Colored divider") ? UbuntuColors.green : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    visible: modelData != i18n.tr("No divider")
                }
            }
        }
    }
}
