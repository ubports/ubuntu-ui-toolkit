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
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    title: "Multi-line List Items"

    Row {
        spacing: 10
        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Multiple values" }

                ListItem.MultiValue {
                    text: "Default"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                }
                ListItem.MultiValue {
                    text: "Disabled"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    enabled: false
                }
                ListItem.MultiValue {
                    text: "Selected"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    selected: true
                }
                ListItem.MultiValue {
                    text: "Progression"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    progression: true
                }
                ListItem.MultiValue {
                    text: "Icon"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    iconSource: "avatar_contacts_list.png"
                }

            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Subtitled" }
                ListItem.Subtitled {
                    text: "Default"
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Disabled"
                    enabled: false
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Selected"
                    selected: true
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Progression"
                    subText: "Secondary label"
                    progression: true
                }

                ListItem.Subtitled {
                    text: "Icon"
                    subText: "Secondary label"
                    iconSource: "avatar_contacts_list.png"
                }
                ListItem.Subtitled {
                    text: "Multiple lines"
                    subText: "This is a multi-line subText.\nUp to 5 lines are supported."
                }
                ListItem.Subtitled {
                    text: "Multiple lines"
                    subText: "It also works well with icons and progression."
                    iconSource: "avatar_contacts_list.png"
                    progression: true
                }
            } // Container
        } // Rectangle
    }
}
