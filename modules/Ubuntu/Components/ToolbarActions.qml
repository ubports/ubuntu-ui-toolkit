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

import QtQuick 2.0

/*!
    \qmltype ActionList
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief List of \l Action items

    Examples: See \l Page.
*/

ActionList {
    /*!
      The back action.
     */
    property Action back: Action {
        iconSource: Qt.resolvedUrl("artwork/back.png")
        text: "Back"
        visible: __pageStack && __pageStack.depth > 1
        /*!
          \internal
          FIXME: If this is not marked as internal, qdoc thinks it needs to be documented.
         */
        onTriggered: __pageStack.pop()
    }

    /*!
      \internal
      PageStack for the back button
     */
    // Cannot use PageStack here that will cause a loop in parsing the qml files
    property Item __pageStack

    /*!
      The toolbar is active
     */
    property bool active: false

    /*!
      The toolbar is locked in its current position.
     */
    property bool lock: false
}
