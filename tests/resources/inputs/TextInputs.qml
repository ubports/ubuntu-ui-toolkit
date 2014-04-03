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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1
import Ubuntu.Components.Popups 0.1

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    applicationName: "TextInputs"

    Flickable {
        anchors.fill: parent
        contentHeight: column.childrenRect.height
        Column {
            id: column
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: units.gu(1)
            TextField {
                objectName: "Standard"
                width: units.gu(30)
                focus: true
                activeFocusOnPress: false
                text: "The orange (specifically, the sweet orange) is the fruit of the citrus species Citrus × ​sinensis in the family Rutaceae."
            }
            TextField {
                objectName: "Preserving"
                width: units.gu(30)
                text: "Second line."
                selectByMouse: false
                placeholderText: "yeeeeewww!"
                persistentSelection: true
            }
            TextArea {
                text: "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
            }
            TextArea {
                text: "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
                persistentSelection: true
            }
            TextArea {
                autoSize: true
                maximumLineCount: 0
            }
            TextArea {
                autoSize: true
                maximumLineCount: 5
            }
        }
    }
}
