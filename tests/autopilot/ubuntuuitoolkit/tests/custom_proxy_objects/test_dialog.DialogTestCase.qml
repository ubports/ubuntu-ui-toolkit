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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
    automaticOrientation: true
    Page {
        Component {
            id: dialog
            Dialog {
                id: dialogue
                title: "Dialog"
                text: "Simple dialog with text area and three stacked button"
                Column {
                    id: row
                    width: parent.width
                    spacing: units.gu(1)
                    TextArea {
                        height: parent.height/2
                        objectName: "textfield_standard"
                        placeholderText: i18n.tr("Type me in...")
                        width: parent.width
                    }
                    Button {
                        width: parent.width
                        text: "Bellow TextField"
                        color: UbuntuColors.green
                        onClicked: PopupUtils.close(dialogue)
                    }
                    Button {
                        width: parent.width
                        text: "Middle button"
                        onClicked: PopupUtils.close(dialogue)
                    }
                    Button {
                        width: parent.width
                        text: "Lower button"
                        onClicked: PopupUtils.close(dialogue)
                    }
                }
            }
        }
        Button {
            objectName: "button"
            anchors.centerIn: parent
            text: i18n.tr("Open dialog")
            onClicked: PopupUtils.open(dialog, null)
        }
    }
}

