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
import Ubuntu.Components 0.1

Template {
    title: "Theming"

    Column {
        spacing: 30

        TemplateRow {
            title: "Current:"

            TextCustom {
                width: 400
                ItemStyle.class: "label"
                text: Theme.currentTheme
            }
        }

        TemplateRow {
            Button {
                text: "Theme #1"
                width: 100
                onClicked: {
                    Theme.loadTheme("demos/themes/global-themes/theme-one/theme.qmltheme")
                }
            }
            Button {
                text: "Theme #2"
                width: 100
                onClicked: {
                    Theme.loadTheme("demos/themes/private-theme.qmltheme")
                }
            }
            Button {
                text: "Default"
                width: 100
                onClicked: {
                    Theme.loadTheme("")
                }
            }
        }

        TemplateRow {
            title: ".Button"

            Button {
                text: "Call"
            }
        }

        TemplateRow {
            title: ".button"

            Button {
                ItemStyle.class: "button"
                text: "Call"
            }
        }

        TemplateRow {
            title: "Custom"

            Item {
                id: frame
                width: 300
                height: 100
                ItemStyle.class: "frame"
                Button {
                    id: customButton
                    x: 10; y: 10; width: 150
                    text: "Private style"
                    ItemStyle.delegate: Item {
                        z: -1
                        anchors.fill: parent
                        clip: true
                        Rectangle {
                            id: shape
                            z: -2
                            anchors.fill: parent
                            anchors.margins: -customButton.width
                            rotation: customButton.pressed ? -45 : 45
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: (ItemStyle.style && ItemStyle.style.color) ? ItemStyle.style.color : "transparent" }
                                GradientStop { position: 1.0; color: customButton.hovered ? "blue" : "red" }
                            }
                        }
                    }
                }
            }
        }
    }
}
