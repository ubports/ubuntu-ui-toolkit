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
    \inqmlmodule Ubuntu.Components 0.1
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
      If no page is specified, the page specified by \l pageSource will be loaded.
     */
    property Item page: loader.item

    /*!
      \preliminary
      pageSource is used as the location of the QML file defining
      the page that will be displayed when this tab is selected and no \l page was defined.
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

        source: (status == Loader.Ready || tab.preloadPage || tab.selected) ? tab.pageSource : ""

        property Item pageParent
        property Item previousPage

        function updatePageVisibility() {
            if (tab.page) {
                if (pageParent) tab.page.parent = pageParent;
                tab.page.visible = tab.selected;
            }
        }

        function hidePreviousPage() {
            if (previousPage && previousPage != tab.page) previousPage.visible = false;
            previousPage = tab.page;
        }

        Connections {
            target: tab
            onPageChanged: {
                loader.hidePreviousPage();
                loader.updatePageVisibility();
            }
            onSelectedChanged: loader.updatePageVisibility()
        }
    }

    /*!
      \internal
      Set the parent of the page to show when this tab is selected.
    */
    function __setPageParent(parentItem) {
        loader.pageParent = parentItem;
    }
}
