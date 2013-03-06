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
import "pageUtils.js" as PageUtils
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
    \endqml

    See also \l Tabs and \l PageStack.
*/
PageTreeNode {
    id: page
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: page.flickable ? parent.height : parent.height - page.headerHeight

    /*!
      The title of the page. Will be shown in the header of the \l MainView.
     */
    property string title

    /*!
      The contents of the header. If this is set, \l title will be ignored.
     */
    property Component headerContents: null

    /*!
      The \l PageStack that this Page has been pushed on, or null if it is not
      part of a PageStack. This value is automatically updated by the \l PageStack.
     */
    property var pageStack

    /*!
      The list of actions associated with this Page.
     */
    property ToolbarActions tools

    /*!
      Optional flickable that controls the header. This property
      is automatically set if the Flickable is one of the Page's direct children.
      May be set to null to avoid the header from hiding.
     */
    property Flickable flickable: PageUtils.getFlickableChild(page)

    Item {
        id: internal

        Connections {
            target: page
            onFlickableChanged: internal.updateFlickableMargins()
            onHeaderHeightChanged: internal.updateFlickableMargins()
        }
        Component.onCompleted: updateFlickableMargins()

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
