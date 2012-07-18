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
    \qmlclass Selector
    \inqmlmodule UbuntuUIToolkit
    \brief List item where the user can select one of multiple values.
    \b{This component is under heavy development.}

    Examples: TODO
*/
Base {
    id: selector
    height: column.height

    property alias text: selectorMain.text
    property alias iconSource: selectorMain.iconSource

    /*!
      \preliminary
      The list of values that will be shown under the label text
     */
    property variant values

    /*!
      \preliminary
      The index of the currently selected element from the \l values array.
     */
    property int selectedIndex: 0

    /*!
      \preliminary
      Specifies whether the selector is 'open' or 'closed'.
     */
    property bool expanded: false

    Column {
        id: column

        SingleValue {
            id: selectorMain
            value: selector.values[selector.selectedIndex]
            progression: true // TODO: replace by accordion icon
            onClicked: selector.expanded = !selector.expanded
            __showTopSeparator:  false // selectors shows this if needed
            __showBottomSeparator: false //selector.expanded // otherwise, selector shows
        }

        Repeater {
            visible: selector.expanded
            height: visible ? 54 : 0
            model: selector.values

            Base {
                id: valueBase
                visible: selector.expanded
                height: visible ? 40 : 0

                onClicked: selector.selectedIndex = index

                Rectangle {
                    color: "#000000"
                    opacity: 0.1
                    anchors.fill: valueBase
                }

                LabelVisual {
                    text: modelData
                    anchors {
                        left: parent.left
                        leftMargin: 24
                        verticalCenter: parent.verticalCenter
                    }
                    font.italic: true
                    font.bold: index === selector.selectedIndex
                }
                __showTopSeparator: true // TODO: show different (smaller) separator
            } // Base
        } // Repeater
    }
}
