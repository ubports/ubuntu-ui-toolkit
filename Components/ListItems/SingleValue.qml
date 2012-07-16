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
import ".."

/*!
    \qmlclass SingleValue
    \inqmlmodule UbuntuUIToolkit
    \brief A list item with a single value

    Examples:
    \qml
        Container {
            SingleValue {
                text: "Label"
                value: "Status"
                onClicked: selected = !selected
            }
            SingleValue {
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
      \qmlproperty bool progression
     */
    property alias progression: progressionHelper.visible

    /*!
      \preliminary
      The values that will be shown next to the label text
     */
    property alias value: valueLabel.text

    Row {
        anchors.fill: parent
        IconVisual { id: iconHelper; height: parent.height }
        Item {
            width: parent.width - iconHelper.width - progressionHelper.width
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            LabelVisual {
                id: label
                selected: listItem.selected
                anchors {
                    verticalCenter: parent.verticalCenter
                    leftMargin: 5
                    left: parent.left
                }
                width: Math.min(invisibleLabel.implicitWidth, parent.width - 10)
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
                    right: parent.right
                    rightMargin: 5
                }
                width: Math.min(invisibleValueLabel.implicitWidth, parent.width - label.width - 15)
                fontSize: "medium"
            }
            LabelVisual {
                id: invisibleValueLabel
                text: valueLabel.text
                visible: false
                elide: Text.ElideNone
            }
        }
        ProgressionVisual {
            id: progressionHelper
            showSplit: true
        }
    }
}
