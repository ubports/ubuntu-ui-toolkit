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

    Examples: See Tabs.
*/
Item {
    id: tab

    /*!
      \preliminary
      The text that is shown on the tab button used to select this tab (optional).
      Either text or iconSource, or both must be defined.
     */
    property string text

    /*!
      \preliminary
      The location of the icon that is displayed inside the button used to select this tab (optional).
      Either text or iconSource, or both must be defined.
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
      If pageSource is used to define the page of this tab, pagePreloaded specifies whether
      the page should be preloaded from the QML file when the application starts, or loading
      is delayed until the tab is selected.
     */
    property bool pagePreloaded


    /*!
      \preliminary
      Indicates whether this tab is selected.
      Its value is automatically updated by the Tabs object.
    */
    property bool selected: false

    Loader {
        id: loader

        function loadPage() {
            loader.source = tab.pageSource;
            tab.page = loader.item;
            loader.item.visible = false;
        }

        Component.onCompleted: {
            if (tab.pagePreloaded) loader.loadPage();
        }
    }

    /*!
      \internal
      Returns the page of this tab.
      If the page property is defined directly, that property is returned.
      Otherwise, the page will be loaded from the file specified by pageSource.
     */
    function __getPage() {
        if (tab.page) return tab.page;
        if (!loader.item) loader.loadPage();
        return loader.item;
    }

    Component.onCompleted: {
        if (tab.page) {
            tab.page.visible = false
        }
    }
}
