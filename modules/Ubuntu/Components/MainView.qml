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
    \qmltype ApplicationWindow
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The root Item for all applications

    \b{This component is under heavy development.}

    Examples:
    \qml
        MainView {
            Button {
                anchors.centerIn: parent
                text: "Click me"
            }
        }
    \endqml
*/
Item {
    /*!
      \preliminary
      The list of actions that will be placed on the toolbar of the application.
     */
    // TODO: Assign the list of actions automatically if the first child of MainView
    //  is an instance of Tabs, PageStack or Page.
    property alias tools: toolbar.tools

    // clip if the MainView is not fullscreen
    clip: true

    /*!
      \internal
      Use default property to ensure children added do not draw over the toolbar.
     */
    default property alias contentsItem: contents.data
    Item {
        id: contents
        anchors.fill: parent
    }

    Toolbar {
        id: toolbar
        tools: getTools()

        function getTools() {
            if (contents.children.length < 1) return null;
            if (!contents.children[0].hasOwnProperty("tools")) return null;
            var tools = contents.children[0].tools;
            if (!tools) return null;
            if (!tools.hasOwnProperty("back")) return null;
            if (!tools.hasOwnProperty("__pageStack")) return null;
            if (!tools.hasOwnProperty("active")) return null;
            if (!tools.hasOwnProperty("lock")) return null;
            return tools;
        }
    }
}
