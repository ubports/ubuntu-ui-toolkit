/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 1.2

MainView {
    width: units.gu(48)
    height: units.gu(60)

    // make sure we're not messed up by the deprecated toolbar
    useDeprecatedToolbar: false

    Page {
        id: testPage
        objectName: "test_page"
        title: listView.ListItem.selectable ? "In selection mode" : "No action triggered"
        ListView {
            id: listView
            anchors.fill: parent
            model: 25
            delegate: ListItem {
                objectName: "listitem" + index
                onPressAndHold: listView.ListItem.selectMode = true
                leadingActions: ListItemActions {
                    actions: [
                        Action {
                            iconName: "delete"
                            objectName: 'delete_action'
                            onTriggered: testPage.title = objectName + " action triggered";
                        }
                    ]
                }
                trailingActions: ListItemActions {
                    actions: [
                        Action {
                            iconName: "search"
                            objectName: 'search_action'
                            onTriggered: testPage.title = objectName + " action triggered";
                        },
                        Action {
                            iconName: "edit"
                            objectName: 'edit_action'
                            onTriggered: testPage.title = objectName + " action triggered";
                        },
                        Action {
                            iconName: "email"
                            objectName: 'email_action'
                            onTriggered: testPage.title = objectName + " action triggered";
                        }
                    ]
                }
            }
        }
    }
}
