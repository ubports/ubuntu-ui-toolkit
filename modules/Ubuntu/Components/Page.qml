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
    \qmltype Page
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief A page inside a \l MainView will have a header and toolbar.
        Pages can also can be included in \l Tab object or pushed on a \l PageStack.
        Anchors and height of a Page are automatically determined to align with
        the header of the \l MainView, but can be overridden.

    Example:
    \qml
        MainView {
            Page {
                title: i18n.tr("Root page")

                tools: ToolbarActions {
                    Action {
                        text: "one"
                        iconSource: Qt.resolvedUrl("1.png")
                        onTriggered: print("First action")
                     }
                     Action {
                        text: "two"
                        iconSource: Qt.resolvedUrl("2.png")
                        onTriggered: print("Second action")
                     }
                }

                Rectangle {
                    anchors.fill: parent
                    color: "red"
                }
            }
        }
    \endqml

    See also \l Tabs and \l PageStack.
*/
PageTreeNode {
    id: page
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        bottom: parent ? parent.bottom : undefined
    }
    height: parent ? page.flickable ? parent.height : parent.height - internal.headerHeight : undefined

    /*!
      The title of the page. Will be shown in the header of the \l MainView.
     */
    property string title

    /*!
      The list of actions associated with this Page.
     */
    property ToolbarActions tools: ToolbarActions { }

    /*!
      Optional flickable that controls the header. This property
      is automatically set if the Flickable is one of the Page's direct children.
      May be set to null to avoid the header from hiding.
     */
    property Flickable flickable: internal.getFlickableChild(page)

    /*! \internal */
    onActiveChanged: internal.updateHeaderAndToolbar()
    /*! \internal */
    onTitleChanged: internal.updateHeaderAndToolbar()
    /*! \internal */
    onHeaderChanged: internal.updateHeaderAndToolbar()
    /*! \internal */
    onToolbarChanged: internal.updateHeaderAndToolbar()
    /*! \internal */
    onToolsChanged: internal.updateHeaderAndToolbar()
    /*! \internal */
    onPageStackChanged: internal.updateHeaderAndToolbar()

    Item {
        id: internal
        function updateHeaderAndToolbar() {
            if (page.active) {
                if (page.header) {
                    page.header.title = page.title;
                    page.header.flickable = page.flickable;
                }
                if (tools) {
                    tools.__pageStack = page.pageStack;
                }
                if (page.toolbar) {
                    page.toolbar.tools = page.tools;
                }
            }
        }

        Connections {
            target: page
            onFlickableChanged: internal.updateFlickableMargins()
        }
        onHeaderHeightChanged: internal.updateFlickableMargins()
        Component.onCompleted: internal.updateFlickableMargins()

        property real headerHeight: page.header ? page.header.height : 0

        function isFlickable(object) {
            return object && object.hasOwnProperty("flicking") && object.hasOwnProperty("flickableDirection");
        }

        /*!
          Return the first flickable child of this page.
         */
        function getFlickableChild(item) {
            if (item && item.hasOwnProperty("children")) {
                for (var i=0; i < item.children.length; i++) {
                    if (internal.isFlickable(item.children[i])) return item.children[i];
                }
            }
            return null;
        }

        function updateFlickableMargins() {
            if (flickable) {
                // Set-up the top-margin of the contents of the Flickable so that
                //  the contents is never hidden by the header:
                page.flickable.contentY = -headerHeight;
                page.flickable.topMargin = headerHeight;
            }
        }
    }
}
