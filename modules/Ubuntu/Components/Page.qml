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
    \qmlclass Page
    \inqmlmodule Ubuntu.Components 0.1
    \brief A page that can be included in Tabs object.

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
Item {
    id: page

    /*!
      \preliminary
      The title that is shown on the tab button used to select this tab (optional).
      Either title or \l iconSource, or both must be defined.
     */
    property string title

    /*!
      \preliminary
      The location of the icon that is displayed inside the button used to select this page (optional).
      Either \l title or iconSource, or both must be defined.
     */
    property url iconSource

    /*!
      \internal
      Used in PageContainer to determine whether an Item is a Page.
     */
    property bool __isPage: true
}
