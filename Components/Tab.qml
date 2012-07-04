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
    \qmlclass Tab
    \inqmlmodule UbuntuUIToolkit
    \brief A child of a Tabs object that represents one tab.

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
Item {
    id: tab

    /*!
      \preliminary
      The text that is shown on the tab button used to select this tab (optional).
      Either text or \l iconSource, or both must be defined.
     */
    property string text

    /*!
      \preliminary
      The location of the icon that is displayed inside the button used to select this tab (optional).
      Either \l text or iconSource, or both must be defined.
     */
    property url iconSource

    /*!
      \preliminary
      The page that is shown when this tab is selected.
      If no page is defined, a pageSource must be given.
     */
    property Item page

    /*!
      \preliminary
      If no page is defined, pageSource is used as the location of the QML file defining
      the page that will be displayed when this tab is selected.
     */
    property url pageSource

    /*!
      \preliminary
      If \l pageSource is used to define the page of this tab, preloadPage specifies whether
      the page should be preloaded from the QML file when the application starts, or loading
      is delayed until the tab is selected.
     */
    property bool preloadPage: false

    /*!
      \preliminary
      Indicates whether this tab is selected.
      Its value is automatically updated by the \l Tabs object.
      TODO: Make readonly when we switch to QtQuick2
    */
    property bool selected: false

    Loader {
        id: loader

        // instead of null, it would be possible to return a "Loading..." page
        property Item activePage: (tab.page) ? tab.page : (loader.item) ? loader.item : null

        property Item activePageParent

        function update() {
            if (tab.preloadPage || tab.selected) loader.source = tab.pageSource;
            if (activePage) {
                if (activePageParent) activePage.parent = activePageParent;
                activePage.visible = tab.selected;
            }
        }

        Connections {
            target: tab
            onPageChanged: loader.update()
            onPreloadPageChanged: loader.update()
            onPageSourceChanged: loader.update()
            onSelectedChanged: loader.update()
        }

        onLoaded: update();
    }

    /*!
      \internal
      Set the parent of the page to show when this tab is selected.
    */
    function __setPageParent(pageParent) {
        loader.activePageParent = pageParent;
    }

    Component.onCompleted: loader.update()
}
