/*
 * Copyright 2014-2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.Styles 1.2
import QtQuick.Layouts 1.1

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(105)

    property bool override: false

    Action {
        objectName: "stock"
        id: stock
        iconName: "starred"
        text: "Staaaar"
        onTriggered: {
            print(iconName, "triggered", value)
            view.ViewItems.selectedIndices = [0, 2, 9];
        }
    }

    ListItemActions {
        id: leading
        objectName: "StockLeading"
        actions: [
            Action {
                iconName: "delete"
                onTriggered: print(iconName, "triggered", value)
            },
            Action {
                iconName: "alarm-clock"
                enabled: false
                onTriggered: print(iconName, "triggered", value)
            },
            Action {
                iconName: "camcorder"
                onTriggered: print(iconName, "triggered", value)
            },
            Action {
                iconName: "stock_website"
                onTriggered: print(iconName, "triggered", value)
            },
            Action {
                iconName: "starred"
                onTriggered: print(iconName, "triggered", value)
            },
            Action {
                iconName: "go-home"
                onTriggered: print(iconName, "triggered", value)
            }
        ]
    }

    property bool selectable: false
    property list<Action> leadingArray: [
        Action {
            iconName: "delete"
            onTriggered: print(iconName, "triggered", value)
        }
    ]
    property list<Action> trailingArray: [
        Action {
            iconName: "search"
            onTriggered: print(iconName, "triggered", value)
        },
        Action {
            iconName: "edit"
            onTriggered: print(iconName, "triggered", value)
        },
        Action {
            iconName: "email"
            onTriggered: print(iconName, "triggered", value)
        }
    ]

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }

        Button {
            text: "Selectable " + (selectable ? "OFF" : "ON")
            onClicked: selectable = !selectable
        }

        ListItem {
            id: testItem
            objectName: "single"
            color: "lime"
            height: units.gu(15)
            onClicked: {
                print("click")
                units.gridUnit += 2;
            }
            onPressAndHold: print("pressAndHold", objectName)
            RowLayout {
                anchors {
                    fill: parent
                    margins: units.gu(0.5)
                    leftMargin: anchors.rightMargin
                    rightMargin: units.gu(2)
                }
                Label {
                    anchors.fill: parent
                    text: units.gridUnit + "PX/unit"
                    Layout.preferredWidth: paintedWidth
                }
                Button {
                    text: "Press me"
                    anchors.centerIn: parent
                    onClicked: units.gridUnit -= 2
                }
            }

            leadingActions: ListItemActions {
                objectName: "InlineLeading"
                actions: [stock]
                delegate: Column {
                    width: height + units.gu(2)
                    Icon {
                        width: units.gu(3)
                        height: width
                        name: action.iconName
                        color: pressed ? "blue" : "pink"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: action.text + index
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
            trailingActions: leading
        }
        ListItem {
            Label {
                id: label
                text: "No action"
            }
            onClicked: print(label.text, "clicked")
        }

        ListItem {
            Label {
                id: label2
                anchors.fill: parent
                text: "Another standalone ListItem\nStarted with custom style, reset to theme style on first click"
            }
            leadingActions: testItem.leadingActions
            trailingActions: ListItemActions {
                actions: leading.actions
            }
            style: ListItemStyle {}
            onClicked: { style = undefined; label2.text = "Another standalone ListItem" }
        }

        ListView {
            id: view
            clip: true
            width: parent.width
            height: units.gu(28)
            model: 25
            pressDelay: 0
            ViewItems.selectMode: main.selectable
            ViewItems.selectedIndices: [9,3,4,1]
            ViewItems.onSelectedIndicesChanged: print("LISTVIEW INDEXES=", ViewItems.selectedIndices)
            delegate: ListItem {
                objectName: "ListItem" + index
                id: listItem
                onClicked: print(" clicked")
                onPressAndHold: print("pressAndHold")
                leadingActions: ListItemActions {
                    actions: trailingArray
                }

                trailingActions: trailing
                contentItem.anchors.margins: units.gu(1)

                RowLayout {
                    anchors {
                        fill: parent
                        margins: units.gu(0.5)
                        leftMargin: anchors.rightMargin
                        rightMargin: units.gu(2)
                    }
                    Captions {
                        title.text: "This is one Label split in two lines.\n" +
                              "The second line - item #" + modelData
                    }
                    Button {
                        text: "Pressme..."
                    }
                }

                states: State {
                    name: "override"
                    when: main.override
                    PropertyChanges {
                        target: listItem
                        highlightColor: "brown"
                    }
                }

                onContentMovementStarted: print("moving started")
                onContentMovementEnded: print("moving ended")
            }
        }
        Flickable {
            id: flicker
            width: parent.width
            height: units.gu(28)
            clip: true
            contentHeight: column.childrenRect.height
            ListItemActions {
                id: trailing
                actions: leading.actions
            }
            Column {
                id: column
                width: flicker.width
                property alias count: repeater.count
                ViewItems.selectMode: main.selectable
                Repeater {
                    id: repeater
                    model: 10
                    ListItem {
                        objectName: "InFlickable"+index
                        color: UbuntuColors.red
                        highlightColor: "lime"
                        divider.colorFrom: UbuntuColors.green

                        leadingActions: ListItemActions {
                            actions: leadingArray
                        }
                        trailingActions: ListItemActions {
                            actions: trailingArray
                        }
                        contentItem.anchors {
                            leftMargin: units.gu(2)
                            rightMargin: units.gu(2)
                        }

                        Label {
                            text: modelData + " Flickable item"
                        }
                        onClicked: divider.visible = !divider.visible
                    }
                }
            }
        }
        ListItem {
            Label {
                text: "Switch makes this item to highlight"
            }
            Switch {
                id: toggle
                anchors.right: parent.right
            }
            Component.onCompleted: clicked.connect(toggle.clicked)
        }
        ListItem {
            Label {
                text: "With action assigned"
            }
            onClicked: print("clicked")
            onPressAndHold: print("longPressed")
            action: stock
        }
    }
}
