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
    \qmlclass Default
    \inqmlmodule UbuntuUIToolkit
    \brief The Default class is DOCME

    \b{This component is under heavy development.}

*/
Base {
    id: listItem
    height: 54

    property alias iconSource: leftVisuals.source
    property alias text: label.text
    property alias progression: progressItem.visible

    property color __textColor: selected ? "#f37505" : Qt.rgba(0.4, 0.4, 0.4, 1.0)

    Row {

        id: visuals
        anchors.fill: parent

        IconHelper {
            id: leftVisuals
        }

//        LabelHelper {
//            id: label
//            selected: listItem.selected
//        }

        Item { // encapsulation
            id: middleVisuals

            height: parent.height
            width: parent.width - leftVisuals.width - progressItem.width


            TextCustom {
                id: label
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 5
                }
                width: parent.width

                fontSize: "large"
                elide: Text.ElideRight

                color: listItem.__textColor
                style: Text.Raised
                styleColor: "white"
                opacity: listItem.enabled ? 1.0 : 0.5
            } // TextCustom
        }

        ProgressionHelper {
            id: progressItem
        }


    } // visuals
}
