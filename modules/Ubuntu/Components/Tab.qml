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

import "Page.js" as Page
/*!
    \qmlclass Tab
    \inqmlmodule Ubuntu.Components 0.1
    \brief Component to represent a single tab in a \l Tabs environment.

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
Item {
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
      The contents of the page. This can also be a string referring to a Page qml file.
      Deactivate the Tab before changing the page, to ensure proper destruction of the
      old page object first, if needed.
     */
    property variant page

    /*!
      \internal
      Specifies whether this tab is the active one. Automatically updated by \l Tabs.
    */
    property bool active: false

    /*!
      \internal
      The Item containing the contents of the tab. This can be the same
      as \l page, if page is an Item, or it can be an object created
      from a QML file if page is a url. Pages loaded from a file are
      automatically destroyed when the tab is no longer active.
    */
    property Item __pageObject

    onPageChanged: {
        if (tab.active) {
            // It is now unclear whether __pageObject should be destroyed
            // here, because possibly ((old)__pageObject !== (new)page), even if
            // __pageObject was created here.
            throw new Error("Deactivate tab before changing the page.");

            // TODO: If it is needed to support changing the page of the active
            // tab, an additional property must be introduced that keeps track of
            // whether __pageObject must be destroyed.
        }
    }

    onActiveChanged: {
        if (tab.active) __pageObject = Page.activate(__pageObject, page, tab);
        else __pageObject = Page.deactivate(__pageObject, page);
    }
}
