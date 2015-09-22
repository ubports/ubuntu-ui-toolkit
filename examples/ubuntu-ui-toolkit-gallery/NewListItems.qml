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
        className: "SlotsLayout (a few examples)"
        // no spacing between the list items in the Column
        spacing: 0

        ListItem {
            id: listItem
            height: layout.height + divider.height

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

            onClicked: icon.SlotsLayout.position = (icon.SlotsLayout.position == SlotsLayout.Leading
                                                    ? SlotsLayout.Trailing
                                                    : SlotsLayout.Leading)
            ListItemLayout {
                id: layout

                title.text: "Hello designers!"
                subtitle.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"

                Icon { id: icon; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(2); name: "email" }
                Icon { width: units.gu(2); name: "message" }
                CheckBox {  }
            }
        }

        ListItem {
            id: listItem2
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

            ListItemLayout {
                id: layout2
                title.text: "Hello designers!"

                Rectangle { SlotsLayout.position: SlotsLayout.Leading; color: "yellow"; width: units.gu(10); height: units.gu(10) }
                CheckBox { }
                Icon { width: units.gu(2); name: "message" }
            }
        }

        ListItem {
            id: listItem3
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

            ListItemLayout {
                id: layout3

                padding.bottom: units.gu(10)

                title.text: "Hello designers!"
                subtitle.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"

                CheckBox { width: units.gu(8); height: units.gu(8) }
                Rectangle { color: "purple"; width: units.gu(5); height: units.gu(5) }
                Icon { SlotsLayout.position: SlotsLayout.Leading; width: units.gu(5); height: units.gu(5); name: "message" }
            }
        }

        ListItem {
            id: listItem4
            height: layout4.height

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

            ListItemLayout {
                id: layout4
                title.text: "Hello designers!"

                CheckBox { width: units.gu(8); height: units.gu(8) }
                Rectangle { color: "orange"; width: units.gu(5); height: units.gu(5) }
                Icon { width: units.gu(5); height: units.gu(5); name: "message" }

            }
        }

        ListItem {
            id: listItem5
            height: layout5.height

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

            ListItemLayout {
                id: layout5
                title.text: "Hello designers!"

                CheckBox { }
            }
        }

        ListItem {
            id: listItem6
            height: layout6.height

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

            ListItemLayout {
                id: layout6
                title.text: "Hello designers!"

                CheckBox { SlotsLayout.position: SlotsLayout.Leading }
            }
        }

        ListItem {
            id: listItem7
            height: layout7.height

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

            ListItemLayout {
                id: layout7
                title.text: "Hello designers!"
                subtitle.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"

                CheckBox { id: checkbox; width: units.gu(4); height: units.gu(4) }

                Rectangle { width: units.gu(4); height: units.gu(4); color: "purple" }

                Icon { SlotsLayout.position: SlotsLayout.Leading; width: units.gu(4); height: units.gu(4); name: "message" }

            }
        }
    }

    TemplateSection {
        className: "SlotsLayout (custom vertical positioning)"
        // no spacing between the list items in the Column
        spacing: 0
        ListItem {
            id: listItemCustom1
            height: listItemCustomLayout1.height

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

            ListItemLayout {
                id: listItemCustomLayout1
                title.text: "Hello designers!"
                subtitle.text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"

                CheckBox { SlotsLayout.overrideVerticalPositioning: true; y: parent.title.y + parent.title.baselineOffset; width: units.gu(4); height: units.gu(4) }

                Rectangle { SlotsLayout.overrideVerticalPositioning: true; y: parent.subtitle.y + parent.subtitle.baselineOffset; width: units.gu(4); height: units.gu(4); color: "purple" }

                Icon { SlotsLayout.overrideVerticalPositioning: true; SlotsLayout.position: SlotsLayout.Leading; anchors.verticalCenter: parent.verticalCenter; width: units.gu(4); height: units.gu(4); name: "message" }
            }
        }
    }


    TemplateSection {
        className: "SlotsLayout (custom vertical positioning)"
        // no spacing between the list items in the Column
        spacing: 0
        ListItem {
            id: listItemCustom2
            height: listItemCustomLayout2.height

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
                id: listItemCustomLayout2


                mainSlot: Item {
                    //even if a Label is empty, its height will still be "lineHeight" value,
                    //and that ruins the layout
                    height: childrenRect.height

                    Label {
                        id: titleLabel
                        width: parent.width
                        height: text != "" ? contentHeight : 0
                        font.weight: Font.Light
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 1
                        text: "Hello designers!"
                    }
                    Label {
                        id: subtitleLabel
                        width: parent.width
                        height: text != "" ? contentHeight : 0
                        anchors {
                            top: titleLabel.text != "" ? titleLabel.baseline : titleLabel.top;
                            topMargin: (text != "" && titleLabel.text != "") ? units.gu(1) : 0
                        }
                        fontSize: "small"
                        font.weight: Font.Light
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        text:  "Once upon a time there was a chicken running on something that fell over something else to create an explosion"
                    }
                }

                Icon { SlotsLayout.overrideVerticalPositioning: true; SlotsLayout.position: SlotsLayout.Leading; anchors.verticalCenter: parent.verticalCenter; width: units.gu(4); height: units.gu(4); name: "message" }
            }
        }
    }

    TemplateSection {
        className: "SlotsLayout (core apps examples)"
        // no spacing between the list items in the Column
        spacing: 0

        ListItem {
            id: addressbookListItem
            height: addressbookLayout.height

            ListItemLayout {
                id: addressbookLayout

                title.color: UbuntuColors.lightAubergine
                title.text: "Address book list item: Name goes here"

                Rectangle {
                    SlotsLayout.position: SlotsLayout.Leading
                    color: "pink"
                    height: units.gu(6)
                    width: height
                    visible: width > 0
                }
            }
        }

        ListItem {
            id: addressbookDetailsListItem
            height: addressbookDetailsLayout.height

            ListItemLayout {
                id: addressbookDetailsLayout

                title.color: UbuntuColors.lightAubergine
                title.text: "+44 7552342512"
                subtitle.text: "Mobile"

                Icon {
                    name: "message"
                    height: units.gu(2)
                }

                Icon {
                    name: "call-start"
                    height: units.gu(2)
                }
            }

        }

        ListItem {
            id: dialerHistoryListItem2
            height: dialerHistoryLayout2.height

            ListItemLayout {
                id: dialerHistoryLayout2

                title.color: UbuntuColors.lightAubergine
                title.text: "+44 7552342512"
                subtitle.text: "Mobile"

                Icon {
                    SlotsLayout.position: SlotsLayout.Leading
                    name: "message"
                    height: units.gu(2)
                }
                Icon {
                    name: "message"
                    height: units.gu(2)
                }

                Icon {
                    name: "call-start"
                    height: units.gu(2)
                }
            }
        }

        ListItem {
            id: dialerHistoryListItem
            height: dialerHistoryLayout.height

            ListItemLayout {
                id: dialerHistoryLayout

                title.text: "+44 7349845928 (3)"
                title.color: UbuntuColors.lightAubergine
                subtitle.text: "Mobile"

                Rectangle {
                    SlotsLayout.position: SlotsLayout.Leading
                    color: "pink"
                    height: units.gu(6)
                    width: height
                    visible: width > 0
                }

                Item {
                    id: slot
                    width: label2.width
                    //don't use childrenRect here because we're positioning the labels in a custom way, the item starts at the top
                    height: label2.y + label2.height

                    //as we want to position labels to align with title and subtitle
                    SlotsLayout.overrideVerticalPositioning: true

                    Label {
                        id: lab
                        anchors.right: label2.right
                        text: "19:17"
                        fontSize: "small"
                        y: dialerHistoryLayout.mainSlot.y + dialerHistoryLayout.title.y
                           + dialerHistoryLayout.title.baselineOffset - baselineOffset
                    }

                    Label {
                        id: label2
                        text: "Outgoing"
                        fontSize: "small"
                        y: dialerHistoryLayout.mainSlot.y + dialerHistoryLayout.subtitle.y
                           + dialerHistoryLayout.subtitle.baselineOffset - baselineOffset
                    }
                }
            }
        }

        ListItem {
            id: telegramContactsListItem

            height: telegramContactsLayout.height

            ListItemLayout {
                id: telegramContactsLayout

                title.text: "Telegram Name goes here"
                title.font.pixelSize: FontUtils.sizeToPixels("large")
                //color: TelegramColors.black
                //verticalAlignment: TextInput.AlignVCenter

                subtitle.text: "last seen 12:02"
                subtitle.font.pixelSize: FontUtils.sizeToPixels("medium")
                //subtitle.color: isOnline ? TelegramColors.dark_blue : TelegramColors.grey
                //subtitle.elide: Text.ElideRight
                //subtitle.verticalAlignment: TextInput.AlignVCenter

                Rectangle {
                    id: imageShape
                    SlotsLayout.position: SlotsLayout.Leading
                    height: units.gu(6)
                    width: height
                    color: "red"
                }
            }
        }

        ListItem {
            id: systemSettings1
            height: systemSettings1_layout.height

            ListItemLayout {
                id: systemSettings1_layout
                title.text: "Call forwarding"

                ProgressionSlot {}

                Text {
                    font.pixelSize: FontUtils.sizeToPixels("medium")
                    text: "Off"
                    color: "#525252"
                }
            }
        }

        ListItem {
            id: systemSettings2
            height: systemSettings2_layout.height

            ListItemLayout {
                id: systemSettings2_layout
                title.text: "Flight mode"

                //see http://bazaar.launchpad.net/~ubuntu-branches/ubuntu/wily/ubuntu-system-settings/wily-proposed/view/head:/src/qml/EntryComponent.qml
                Icon {
                    SlotsLayout.position: SlotsLayout.Leading
                    width: units.gu(6);
                    height: units.gu(6);
                    name: "airplane-mode"
                }
                Switch { }
            }
        }

        ListItem {
            id: systemSettings3
            height: systemSettings3_layout.height

            ListItemLayout {
                id: systemSettings3_layout
                title.text: "الفيسبوك"
                subtitle.text: "الفيسبوك"

                LayoutMirroring.enabled: true
                LayoutMirroring.childrenInherit: true

                //ProgressionSlot {}

                //see http://bazaar.launchpad.net/~ubuntu-branches/ubuntu/wily/ubuntu-system-settings/wily-proposed/view/head:/src/qml/EntryComponent.qml
                Icon {
                    SlotsLayout.position: SlotsLayout.Leading
                    width: units.gu(6);
                    height: units.gu(6);
                    name: "facebook"
                }
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
