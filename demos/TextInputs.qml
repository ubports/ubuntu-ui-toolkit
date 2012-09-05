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
    title: "Text Input"
    id: indicators

    Column {
        spacing: 30
        TemplateRow {
            title: "Basic"

            TextField {
                placeholderText: "simple text field"
                hasClearButton: false
            }
        }

        TemplateRow {
            title: "Clear button"

            TextField {
                placeholderText: "type to show clear button"
                text: "sample text"
            }
        }

        TemplateRow {
            title: "Password"

            TextField {
                placeholderText: "password with echo"
                echoMode: TextInput.Password

            }
        }

        TemplateRow {
            title: "Password"

            TextField {
                placeholderText: "password no echo"
                echoMode: TextInput.NoEcho

            }
        }

        TemplateRow {
            title: "Password"

            TextField {
                placeholderText: "password echo on editing"
                echoMode: TextInput.PasswordEchoOnEdit

            }
        }

        TemplateRow {
            title: "Overlaid"

            TextField {

                leftView: Image {
                    source: "call_icon.png"
                    smooth: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: console.log("click")
                    }
                }
                rightView: Image {
                    source: "avatar_contacts_list.png"
                    smooth: true
                    height: parent.height - 4
                    MouseArea {
                        anchors.fill: parent
                        onClicked: console.log("clack")
                    }
                }
            }
        }
    }
}
