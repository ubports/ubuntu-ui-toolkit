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
    objectName: "Theming"
    title: "Theming"

    Column {
        spacing: units.gu(4)

        TemplateRow {
            title: "Custom"
            Switch {
                Component.onCompleted: checked = (Theme.currentTheme.indexOf("custom-theme") > 0)
                onCheckedChanged: {
                    if (checked)
                        Theme.loadTheme(Qt.resolvedUrl("./custom-theme.qmltheme"));
                    else
                        Theme.loadTheme("");
                }
            }
        }

        TemplateRow {
            title: "Button"
            height: code1.height

            Button {
                id: button
                text: "Call"
                Binding {
                    target: button.ItemStyle.style
                    property: "color"
                    value: "green"
                }
            }

            Label {
                ItemStyle.class: "row-label"
                text: " code "
            }

            Rectangle {
                id: code1
                height: codeSnipet1.paintedHeight + units.gu(2)
                width: codeSnipet1.paintedWidth + units.gu(2)
                color: "#efefef"
                radius: units.gu(1)
                border.color: Qt.darker(color, 1.2)
                Label {
                    anchors.margins: units.gu(1)
                    id: codeSnipet1
                    ItemStyle.class: "row-label"
                    anchors.fill: parent
                    text: "Button {\n"+
                          "    id: button\n"+
                          "    text: \"Call\"\n"+
                          "    Binding {\n"+
                          "        target: button.ItemStyle.style\n"+
                          "        property: \"color\"\n"+
                          "        value: \"green\"\n"+
                          "    }\n"+
                          "}"
                }
            }
        }

        TemplateRow {
            title: "Custom style"
            height: code2.height

            Button {
                ItemStyle.class: "custom-button"
                text: "Call"
            }
            Label {
                ItemStyle.class: "row-label"
                text: " code "
            }

            Rectangle {
                id: code2
                height: codeSnipet2.paintedHeight + units.gu(2)
                width: codeSnipet2.paintedWidth + units.gu(2)
                color: "#efefef"
                radius: units.gu(1)
                border.color: Qt.darker(color, 1.2)
                Label {
                    anchors.margins: units.gu(1)
                    id: codeSnipet2
                    ItemStyle.class: "row-label"
                    anchors.fill: parent
                    text: "Button {\n"+
                          "    ItemStyle.class: \"custom-button\"\n"+
                          "    text: \"Call\"\n"+
                          "}"
                }
            }
        }

        TemplateRow {
            title: "Custom style"
            height: code3.height

            Button {
                ItemStyle.name: "custom-name"
                text: "Call"
            }
            Label {
                ItemStyle.class: "row-label"
                text: " code "
            }

            Rectangle {
                id: code3
                height: codeSnipet3.paintedHeight + units.gu(2)
                width: codeSnipet3.paintedWidth + units.gu(2)
                color: "#efefef"
                radius: units.gu(1)
                border.color: Qt.darker(color, 1.2)
                Label {
                    anchors.margins: units.gu(1)
                    id: codeSnipet3
                    ItemStyle.class: "row-label"
                    anchors.fill: parent
                    text: "Button {\n"+
                          "    ItemStyle.name: \"custom-name\"\n"+
                          "    text: \"Call\"\n"+
                          "}"
                }
            }
        }
    }
}
