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
    \qmltype Action
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Describe an action that can be re-used in, for example a \l Button

    \b{This component is under heavy development.}

    Examples: See \l Page
*/
QtObject {
    id: action

    /*!
      \preliminary
      The title of the action.
     */
    property string text

    /*!
      \preliminary
      The image associated with the action.
     */
    property url iconSource

    /*!
      Called when the action is triggered.
     */
    signal triggered(var caller)

    /*!
      The action is visible to the user
     */
    property bool visible: true

    /*!
      Enable the action. It may be visible, but disabled.
     */
    property bool enabled: true
}
