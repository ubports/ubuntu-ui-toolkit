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
    \qmltype Tab
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Component to represent a single tab in a \l Tabs environment.

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
//Item {
PageTreeNode {
    id: tab

    anchors.fill: parent ? parent : undefined

    /*!
      \preliminary
      The title that is shown on the tab button used to select this tab (optional).
      Either title or \l iconSource, or both must be defined.
     */
    property string title

    /*!
      \preliminary
      The location of the icon that is displayed inside the button used to select this tab (optional).
      Either \l title or iconSource, or both must be defined.
     */
    property url iconSource

    /*!
      \preliminary
      The contents of the page. This can also be a string referring to a qml file.
      Deactivate the Tab before changing the page, to ensure proper destruction of the
      old page object first, if needed.
     */
    property Item page: null


    onPageChanged: if (page) page.parent = tab

    active: parentNode && parentNode.hasOwnProperty("selectedTab") && parentNode.selectedTab === tab
}
