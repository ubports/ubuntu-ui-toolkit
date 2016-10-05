/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3

Template {
    objectName: "toolbarTemplate"
    id: toolbarTemplate

    property list<Action> actionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
        },
        Action {
            iconName: "appointment"
            text: "Date"
        },
        Action {
            iconName: "attachment"
            text: "Attach"
        },
        Action {
            iconName: "contact"
            text: "Contact"
        },
        Action {
            iconName: "like"
            text: "Like"
        },
        Action {
            iconName: "lock"
            text: "Lock"
        }
    ]

    property list<Action> shortActionList: [
        Action {
            iconName: "share"
            text: "Share"
        },
        Action {
            iconName: "starred"
            text: "Favorite"
        }
    ]

    property list<Action> longActionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
            onTriggered: print("tock")
        },
        Action {
            iconName: "appointment"
            text: "Date"
            onTriggered: print("date")
        },
        Action {
            iconName: "attachment"
            text: "Attach"
            onTriggered: print("attach")
        },
        Action {
            iconName: "contact"
            text: "Contact"
            onTriggered: print("contact")
        },
        Action {
            iconName: "like"
            text: "Like"
            onTriggered: print("+1")
        },
        Action {
            iconName: "lock"
            text: "Lock"
            onTriggered: print("lock")
        },
        Action {
            iconName: "camcorder"
            text: "Camera"
            onTriggered: print("cam")
        },
        Action {
            iconName: "location"
            text: "Location"
            onTriggered: print("loc")
            enabled: false
        },
        Action {
            iconName: "message"
            text: "Message"
            onTriggered: print("msg")
        },
        Action {
            iconName: "livetv"
            text: "Television"
            onTriggered: print("tv")
        },
        Action {
            iconName: "lock-broken"
            text: "Unlock"
            onTriggered: print("unlock")
        }
    ]

    property list<Action> deleteActionList: [
        Action {
            iconName: "delete"
            text: "Delete"
            onTriggered: print("delete")
        }

    ]

    header: PageHeader {
        id: h
        title: toolbarTemplate.title
        extension: Toolbar {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            leadingActionBar.actions: deleteActionList
            trailingActionBar.actions: longActionList
        }
        trailingActionBar.actions: shortActionList
    }

    TemplateSection {
        title: "Toolbar"
        className: "Toolbar"

        TemplateRow {
            width: parent.width
            title: i18n.tr("Enabled")
            Toolbar {
                width: parent.width
                trailingActionBar.actions: actionList
                leadingActionBar.actions: deleteActionList
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")
            Toolbar {
                width: parent.width
                trailingActionBar.actions: actionList
                leadingActionBar.actions: deleteActionList
                enabled: false
            }
        }
    }

    TemplateSection {
        title: "Scrollable toolbar"
        className: "Toolbar"

        TemplateRow {
            title: i18n.tr("Enabled")
            Toolbar {
                width: parent.width
                trailingActionBar.actions: longActionList
                leadingActionBar.actions: deleteActionList
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")
            Toolbar {
                width: parent.width
                trailingActionBar.actions: longActionList
                leadingActionBar.actions: deleteActionList
                enabled: false
            }
        }
    }
}
