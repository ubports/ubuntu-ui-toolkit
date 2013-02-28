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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

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
    id: mainView

    /*!
      \preliminary
      The property holds the application's name, which must be the same as the
      desktop file's name.
      */
    property string applicationName

    // FIXME: Make sure that the theming is only in the background, and the delegate
    //  should not occlude contents of the MainView. When making changes here, make
    //  sure that bug https://bugs.launchpad.net/manhattan/+bug/1124076 does not come back.
    Item {
        id: background
        Theming.ItemStyle.class: "mainview"
        anchors.fill: parent
    }

    /*!
      \preliminary
      The list of actions that will be placed on the toolbar of the application.
     */
    property alias tools: toolbar.tools
    //    property ToolbarActions tools

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

    property var activePage: null

    property alias header: header
    Header {
        id: header
        title: activePage && activePage.hasOwnProperty("title") ? activePage.title : ""
        flickable: activePage && activePage.hasOwnProperty("flickable") ? activePage.flickable : null
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

        onActiveChanged: if (active) header.show()
    }

    property QtObject __hud: null
    /*! \internal */
    onApplicationNameChanged: {
        if (applicationName !== "") {
            var component = Qt.createComponent(Qt.resolvedUrl("HudIntegration.qml"));
            if (component)
                __hud = component.createObject(mainView, {"applicationIdentifier": applicationName});
        } else if (__hud)
            __hud.destroy();
    }

    /*!
      \internal
      Internal helper property to indicate that this is a MainView
     */
    property bool __isMainView: true
}
