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

/*!
    \qmlclass SingleValue
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief A list item displaying a single value

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        ListItem.Container {
            ListItem.SingleValue {
                text: "Label"
                value: "Status"
                onClicked: selected = !selected
            }
            ListItem.SingleValue {
                text: "Label"
                iconSource: "icon.png"
                value: "Parameter"
                progression: true
                onClicked: print("clicked")
            }
        }
    \endqml

    \b{This component is under heavy development.}

*/
Selectable {
    id: listItem
    height: 54

    /*!
      \preliminary
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    property alias iconSource: iconHelper.source

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: label.text

    /*!
      \preliminary
      Show or hide the progression symbol.
     */
    property bool progression: false

    /*!
      \preliminary
      The values that will be shown next to the label text
     */
    property alias value: valueLabel.text

    IconVisual {
        id: iconHelper;
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }
    LabelVisual {
        id: label
        selected: listItem.selected
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: 5
            left: iconHelper.right
        }
        width: Math.min(invisibleLabel.implicitWidth, listItem.width - iconHelper.width - progressionHelper.width)
    }
    LabelVisual {
        id: invisibleLabel
        visible: false
        text: label.text
        elide: Text.ElideNone
    }
    LabelVisual {
        id: valueLabel
        selected: listItem.selected
        anchors {
            verticalCenter: parent.verticalCenter
            right: progressionHelper.left
            rightMargin: 5
            left: label.right
            leftMargin: 5
        }
        horizontalAlignment: Text.AlignRight
        fontSize: "medium"
    }
    ProgressionVisual {
        id: progressionHelper
        visible: listItem.progression
        showSplit: true
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
}
