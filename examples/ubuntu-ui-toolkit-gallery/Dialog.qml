/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components.Popups 1.3

Template {
    objectName: "dialogsTemplate"

    TemplateSection {
        className: "Dialog"
        documentation: "qml-ubuntu-components-popups-%1.html".arg(className.toLowerCase())

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                text: i18n.tr("Open")
                width: units.gu(16)
                onClicked: PopupUtils.open(dialog, null)
            }
        }

        Component {
            id: dialog
            Dialog {
                id: dialogue

                title: "Sample Dialog"
                text: "Descriptive text goes here"

                TextField {
                    placeholderText: "Type like your lifes depends on it"
                }

                Row {
                    id: row
                    width: parent.width
                    spacing: units.gu(1)
                    Button {
                        width: parent.width/2
                        text: "Cancel"
                        onClicked: PopupUtils.close(dialogue)
                    }
                    Button {
                        width: parent.width/2
                        text: "Confirm"
                        color: UbuntuColors.green
                        onClicked: PopupUtils.close(dialogue)
                    }
                }
            }
        }
    }
}
