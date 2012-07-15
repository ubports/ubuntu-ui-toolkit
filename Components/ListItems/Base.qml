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
    \qmlclass Base
    \inqmlmodule UbuntuUIToolkit
    \brief The parent of all list items

    \b{This component is under heavy development.}

*/
Item {
    id: baseListItem
    width: 250

    signal clicked

    /*!
      \internal
     */
    property bool __isFirst: false

    /*!
      \internal
     */
    property bool __isLast: false

    AbstractButton {
        anchors.fill: parent
        enabled: baseListItem.enabled
        onClicked: baseListItem.clicked()
    }
}
