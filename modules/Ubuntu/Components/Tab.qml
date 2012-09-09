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
    \brief TODO

    \b{This component is under heavy development.}

    Examples: See \l Tabs.
*/
Item {
    id: tab

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

    property bool __isTab: true

    /*!
      \preliminary
      TODO. URL (string) or a Page object.
     */
    property variant page

    property Page __pageObject

    onPageChanged: {
        var pageObject;
        pageObject = tab.__initPage(tab.page);
        tab.__pageObject = pageObject;
    }

    // TODO: add properties?
    function __initPage(page) {
        var containerComponent = tab; //Item.createComponent(tab);

        var pageComponent;
        if (page.createObject) {
            // page is defined as a component
            pageComponent = page;
        }
        else if (typeof page =="string") {
            // page is defined as a string (url)
            pageComponent = Qt.createComponent(page);
        }

        var pageObject;
        if (pageComponent) {
            if (pageComponent.status == Component.Error) {
                throw new Error("Error while loading page: " + pageComponent.errorString());
            } else {
                // TODO: add parent (container), and properties
                pageObject = pageComponent.createObject(containerComponent);
            }
        } else {
            pageObject = page;
        }

        if (pageObject.parent != containerComponent) {
            pageObject.parent = containerComponent;
        }

        return pageObject;
    }
}
