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

/*!
    \qmlclass MultiValue
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief List item displaying a second string under the main label.
    \b{This component is under heavy development.}

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Subtitled {
                text: "Idle"
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
                icon: Qt.resolvedUrl("icon.png")
            }
            ListItem.Subtitled {
                text: "Multiple lines"
                subText: "This is a multi-line subText.\nUp to 5 lines are supported."
            }
            ListItem.Subtitled {
                text: "Multiple lines"
                subText: "It also works well with icons and progression."
                icon: Qt.resolvedUrl("icon.png")
                progression: true
            }
        }
    \endqml
*/
Base {
    id: subtitledListItem
    __height: Math.max(middleVisuals.height, 48)

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: label.text

    /*!
      \preliminary
      The list of strings that will be shown under the label text
      \qmlproperty string subText
     */
    property alias subText: subLabel.text

    Item  {
        id: middleVisuals
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: __verticalCenterOffset
        }
        height: childrenRect.height + label.anchors.topMargin + subLabel.anchors.bottomMargin

        LabelVisual {
            id: label
            selected: subtitledListItem.selected
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 5
                rightMargin: 5
                topMargin: 5
            }
        }
        LabelVisual {
            id: subLabel
            selected: subtitledListItem.selected
            anchors {
                leftMargin: 5
                bottomMargin: 5
                rightMargin: 5

                left: parent.left
                right: parent.right
                top: label.bottom
            }
            fontSize: "small"
            wrapMode: Text.Wrap
            maximumLineCount: 5
        }
    }
}
