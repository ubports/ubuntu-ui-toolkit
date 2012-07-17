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
    \qmlclass MultiValue
    \inqmlmodule UbuntuUIToolkit
    \brief List item with multiple values.
    \b{This component is under heavy development.}

    Examples:
    \qml
        Container {
            TODO
        }
    \endqml
*/
Selectable {
    id: subtitledListItem
    height: middleVisuals.height + 10//54

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
      The list of strings that will be shown under the label text
     */
    property variant subText

    Row {
        // anchors.fill: parent
        //height: childrenRect.height
        width: parent.width
        Item {
            width: iconHelper.width
            height: iconHelper.height
            IconVisual { id: iconHelper; height: 46 }
        }
        Column  {
            id: middleVisuals
            width: parent.width - iconHelper.width - progressionHelper.width
            Item {
                id: spacing
                width: parent.width
                height: 5
            }

            LabelVisual {
                id: label
                selected: subtitledListItem.selected
                anchors {
                   // top: parent.top
                    leftMargin: 5
                    left: parent.left
                }
                width: parent.width
            }

            Repeater {
                model: subtitledListItem.subText
                TextCustom {
                    id: subLabel
                    property bool selected: subtitledListItem.selected

                    fontSize: "medium"
                    color: selected ? "#f37505" : Qt.rgba(0.4, 0.4, 0.4, 1.0)
                    style: Text.Raised
                    styleColor: "white"
                    opacity: label.enabled ? 1.0 : 0.5

                    anchors {
                        leftMargin: 5
                        left: parent.left
                    }

                    width: parent.width
                    text: modelData

                    elide: Text.ElideRight
                }
            }
        } // middleVisuals
        ProgressionVisual {
            id: progressionHelper
            showSplit: true
        }
    }
}
