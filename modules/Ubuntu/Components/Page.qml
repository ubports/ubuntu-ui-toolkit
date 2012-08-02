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
      \preliminary
      The Item that is shown when this page is active.
      If no contents is specified, the contents specified by \l contentsSource will be loaded.
     */
    property Item contents: loader.item

    /*!
      \preliminary
      conentsSource is used as the location of the QML file defining
      the contents that will be displayed when this page is active and no \l contents was defined.
     */
    property url contentsSource

    /*!
      \preliminary
      If \l contentsSource is used to define the page of this tab, preloadContents specifies whether
      the contents should be preloaded from the QML file when the application starts, or loading
      is delayed until the contents is active.
     */
    property bool preloadContents: false

    /*!
      \preliminary
      Indicates whether this page is active.
      Its value is automatically updated by the \l Tabs object.
      TODO:  make internal and prefix with __?
    */
    property bool active: false

    /*!
      \preliminary
      Specifies the visual parent of the \l contents Item.
      The parent of \l contents is set when the page is made active
      (after it is automatically loaded from \l contentsSource, if applicable).
      This value is automatically updated by the \l Tabs object.
      TODO: make internal and prefix with __?
     */
    property Item contentsParent

    /*!
      \internal
      Used in PageContainer to determine whether an Item is a Page.
     */
    property bool __isPage: true

    Loader {
        id: loader

        source: (status == Loader.Ready || page.preloadContents || page.active) ? page.contentsSource : ""

        property Item previousContents

        function updateContentsVisibility() {
            if (page.contents) {
                if (page.contentsParent) page.contents.parent = page.contentsParent;
                page.contents.visible = page.active;
                print("Updated visibility for " + page.title + " to "+ page.active);
            }
        }

        function hidePreviousContents() {
            if (previouscontents && previousContents !== page.contents) previousContents.visible = false;
            previousContents = page.contents;
        }

        Connections {
            target: page
            onContentsChanged: {
                loader.hidePreviousContents();
                loader.updateContentsVisibility();
            }
            onActiveChanged: loader.updateContentsVisibility()
        }
    }
}
