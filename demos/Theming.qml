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

import QtQuick 1.1
import Ubuntu.Components 0.1

Template {
    title: "Theming"

    Column {
        spacing: 30

        TemplateRow {
            Button {
                text: "Theme #1"
                width: 100
                onClicked: theme.loadTheme("SampleTheme1.qml")
            }
            Button {
                text: "Theme #2"
                width: 100
                onClicked: theme.loadTheme("SampleTheme2.qml")
            }
        }

        TemplateRow {
            title: "Standard"

            Button {
                //styleClass: "button"
                text: "Call"
            }
        }

        TemplateRow {
            title: "Custom props"

            Button {
                styleClass: "button"
                text: "Call"
            }
        }
        TemplateRow {
            title: "Custom props"

            StyledItem {
                id: frame
                width: 300
                height: 100
                styleClass: "tframe"
                Rectangle {
                    anchors.fill: parent
                    border.color: frame.styleObject.border
                    border.width: 2
                    radius: 3
                    color: frame.styleObject.color
                }

                Button {
                    styleClass: "tbutton"
                    text: "Call"
                }
            }
        }
    }
}
