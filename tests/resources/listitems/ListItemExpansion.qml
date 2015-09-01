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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Action {
        id: deleteAction
        iconName: "delete"
    }
    property list<Action> contextualActions: [
        Action {
            iconName: "edit"
        },
        Action {
            iconName: "share"
        },
        Action {
            iconName: "stock_website"
        }
    ]
    Page {
        title: "Expansion"
        Column {
            anchors.fill: parent
            spacing: units.dp(4)
            UbuntuListView {
                onEnabledChanged: print("enabled", enabled)
                width: parent.width
                height: parent.height / 2
                clip: true
                ViewItems.onExpandedIndicesChanged: print(ViewItems.expandedIndices)

                model: ListModel {
                    Component.onCompleted: {
                        for (var i = 0; i < 3; i++) {
                            append({label: "List item #"+i, sectionData: "Locked"});
                        }
                        for (i = 3; i < 11; i++) {
                            append({label: "List item #"+i, sectionData: "Limited, live move"});
                        }
                        for (i = 11; i < 25; i++) {
                            append({label: "List item #"+i, sectionData: "Unlimited, drag'n'drop"});
                        }
                    }
                }

                delegate: ListItem {
                    id: item
                    objectName: "ListItem-" + index
                    leadingActions: ListItemActions {
                        actions: deleteAction
                    }
                    trailingActions: ListItemActions {
                        actions: contextualActions
                    }

                    RowLayout {
                        anchors {
                            fill: parent
                            margins: units.gu(0.5)
                            leftMargin: anchors.rightMargin
                            rightMargin: units.gu(2)
                        }
                        Captions {
                            id: captions
                            title.text: label
                            subtitle.text: "from index #" + index
                        }
                        Captions {
                            captionStyle: Ubuntu.SummaryCaptionStyle
                            title.text: "LTR"
                            subtitle.text: "RTL"
                        }
                    }

                    onPressAndHold: {
                        print("expand/collapse")
                        expansion.height = units.gu(15)
                        expansion.expanded = !expansion.expanded
                    }
                }
            }
            ListItem {
                Label { text: "Standalone ListItem" }
                expansion.height: units.gu(15)
                onPressAndHold: expansion.expanded = !expansion.expanded;
            }
        }
    }
}
