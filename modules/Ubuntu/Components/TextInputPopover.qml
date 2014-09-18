/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0

Popover {
    id: popover
    objectName: "text_input_contextmenu"
    property Item target
    property list<Action> actions: [
        Action {
            text: i18n.dtr('ubuntu-ui-toolkit', "Select All")
            iconName: "edit-select-all"
            visible: target && target.selectedText === ""
            onTriggered: target.selectAll()
        },
        Action {
            text: i18n.dtr('ubuntu-ui-toolkit', "Cut")
            iconName: "edit-cut"
            // If paste/editing is not possible, then disable also "Cut" operation
            // It is applicable for ReadOnly's TextFields and TextAreas
            enabled: target && target.selectedText !== "" && target.canPaste
            visible: target.selectedText !== ""
            onTriggered: target.cut()
        },
        Action {
            text: i18n.dtr('ubuntu-ui-toolkit', "Copy")
            iconName: "edit-copy"
            enabled: target && target.selectedText !== ""
            visible: target.selectedText !== ""
            onTriggered: target.copy()
        },
        Action {
            text: i18n.dtr('ubuntu-ui-toolkit', "Paste")
            iconName: "edit-paste"
            enabled: target && target.canPaste
            onTriggered: target.paste()
        }
    ]

    contentHeight: row.childrenRect.height
    contentWidth: row.childrenRect.width
    Row {
        id: row
        height: units.gu(6)

        Repeater {
            model: actions.length
            AbstractButton {
                id: button
                /*
                  Workaround for autopilot used in the text input's context menu to access
                  action.text so we can get the proper button by text, action being not
                  accessible. https://bugs.launchpad.net/autopilot/+bug/1334599
                  */
                property string text: action.text
                width: Math.max(units.gu(4), implicitWidth) + units.gu(2)
                height: units.gu(6)
                action: actions[modelData]
                style: Theme.createStyleComponent("ToolbarButtonStyle.qml", button)
                onClicked: popover.hide()
            }
        }
    }
}
