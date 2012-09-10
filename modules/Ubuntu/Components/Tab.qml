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
     */
    property variant page

    /*!
      \internal
      Specifies whether this tab is the active one. Set by Tabs.
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
        if (tab.__pageObject && tab.__pageObject !== page) {
            tab.__pageObject.destroy();
            tab.__pageObject = null;
        }
        if (tab.active) tab.__pageObject = tab.__initPage(tab.page);
    }

    onActiveChanged: {
        if (!__pageObject && tab.active) {
            __pageObject = __initPage(tab.page);
            __pageObject.anchors.fill = tab;
        }
        if (__pageObject) __pageObject.visible = tab.active;
        if (!tab.active &&  __pageObject && __pageObject !== page ) {
            __pageObject.destroy();
            __pageObject = null;
        }
    }

    /*!
      \internal
      Initialize __pageObject.
     */
    function __initPage(page) {
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
                pageObject = pageComponent.createObject(tab);
            }
        } else {
            pageObject = page;
        }

        if (pageObject.parent !== tab) {
            pageObject.parent = tab;
        }

        return pageObject;
    }
}
