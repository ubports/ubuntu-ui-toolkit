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

/*!
    \qmlclass AnimatedItem
    \inqmlmodule Ubuntu.Components 0.1
    \brief The AnimatedItem drives the animated components behavior inside a Flickable.

*/

import QtQuick 1.1

Item {
    id: root
    /*!
      \preliminary
      Specifies whether the component is on the visible area of the Flickable or not.
    */
    property bool onScreen: true

    QtObject {
        id: internal
        property Flickable flickable
    }

    Connections {
        target: internal.flickable

        onMovementStarted: root.onScreen = true

        onMovementEnded: {
            var pos = mapToItem(internal.flickable, 0, 0)
            root.onScreen = (pos.y + root.height >= 0) || (pos.y <= internal.flickable.height) ||
                            (pos.x + root.width >= 0) || (pos.x <= internal.flickable.width)
        }
    }

    Component.onCompleted: {
        // find a flickable parent
        var flickable = root.parent
        while (flickable) {
            if (flickable.hasOwnProperty("flicking") && flickable.hasOwnProperty("flickableDirection"))
                break
            flickable = flickable.parent
        }
        if (flickable)
            internal.flickable = flickable
    }
}
