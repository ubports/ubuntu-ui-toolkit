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
    title: "Text Input"
    id: page

    Item {
        MouseArea {
            anchors.fill: parent
            onClicked: page.forceActiveFocus()
        }
        Column {
            spacing: units.dp(30)

            TemplateRow {
                title: "Basic"

                TextField {
                    id: textField1
                    KeyNavigation.tab: textField2
                    KeyNavigation.backtab: textField5

                    placeholderText: "simple text field"
                    hasClearButton: false
                }
            }

            TemplateRow {
                title: "Clear button"

                TextField {
                    id: textField2
                    KeyNavigation.tab: textField3
                    KeyNavigation.backtab: textField1

                    placeholderText: "type to show clear button"
                    text: "sample text"
                }
            }

            TemplateRow {
                title: "Password"

                TextField {
                    id: textField3
                    KeyNavigation.tab: textField4
                    KeyNavigation.backtab: textField2

                    placeholderText: "password with echo"
                    echoMode: TextInput.Password

                }
            }

            TemplateRow {
                title: "Password"

                TextField {
                    id: textField4
                    KeyNavigation.tab: textField5
                    KeyNavigation.backtab: textField3

                    placeholderText: "password echo on editing"
                    echoMode: TextInput.PasswordEchoOnEdit

                }
            }

            TemplateRow {
                title: "Overlaid"

                TextField {
                    id: textField5
                    KeyNavigation.tab: textField1
                    KeyNavigation.backtab: textField4

                    primaryItem: ButtonWithForeground {
                        iconSource: "call_icon.png"
                        width: units.dp(20)
                    }
                    secondaryItem: ButtonWithForeground {
                        iconSource: "avatar_contacts_list.png"
                        width: height
                        height: parent.height - units.dp(5)
                    }
                }
            }
        }
    }
}
