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

import QtQuick 2.0
import Ubuntu.Components 1.0
import Ubuntu.Components.Popups 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Button {
        objectName: "open_popover"
        text: "Open Popover"
        onClicked: testActionsPopover.show();
    }

    Label {
        id: "label"
        objectName: "clicked_label"
        anchors.centerIn: parent
        text: "Button not clicked."
    }

    ActionSelectionPopover {
        objectName: "test_actions_popover"
        id: testActionsPopover
        actions: ActionList {
            Action {
                text: "Action one"
                objectName: "actionOne"
                onTriggered: label.text = "Button clicked."
            }
            Action {
                text: "Action two"
                objectName: "actionDisabled"
                enabled: false
                onTriggered: label.text = "Disabled button clicked."
            }
            Action {
                text: "Action three"
                objectName: "actionHidden"
                visible: false
                onTriggered: label.text = "Hidden button clicked."
            }
        }
    }
}
