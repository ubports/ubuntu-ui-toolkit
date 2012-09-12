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
    \qmlclass PageWrapper
    \inqmlmodule Ubuntu.Components 0.1
    \brief TODO

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
QtObject {
    /*!
      \preliminary
      The reference to the page object. This can be the page
      itself, but also a url pointing to a QML file.
     */
    property variant reference

    /*!
      \preliminary
      The initialized page object, or null if the object needs to be created.
     */
    property Item object

    /*!
      \preliminary
      The parent Item of the page object.
     */
    property Item owner
}
