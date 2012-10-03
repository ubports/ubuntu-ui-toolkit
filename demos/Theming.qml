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
            title: "Current:"

            TextCustom {
                styleClass: "label"
                text: theme.currentTheme
            }
        }

        TemplateRow {
            Button {
                text: "Theme #1"
                width: 100
                onClicked: theme.setTheme("demos/test-theme1.css")
            }
            Button {
                text: "Theme #2"
                width: 100
                onClicked: theme.setTheme("demos/test-theme2.css")
            }
            Button {
                text: "Default"
                width: 100
                onClicked: theme.setTheme("")
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
                styleClass: "button"
                text: "Call"
            }
        }
        TemplateRow {
            title: "Custom"

            StyledItem {
                id: frame
                width: 300
                height: 100
                styleClass: "frame"

                Button {
                    x: 10; y: 10
                    text: "Call"
                    styleRule: Rule {
                        delegate: Item {
                            z: -1
                            anchors.fill: widget
                            clip: true
                            Rectangle {
                                id: shape
                                anchors.fill: parent
                                anchors.margins: -widget.width
                                rotation: widget.pressed ? -45 : 45
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: "lightsteelblue" }
                                    GradientStop { position: 1.0; color: widget.hovered ? "blue" : "green" }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
