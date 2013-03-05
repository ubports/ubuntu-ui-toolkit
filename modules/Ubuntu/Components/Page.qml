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
Item {
    id: page

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: flickable ? parent.height : parent.height - internal.headerHeight

    /*!
      The title of the page. Will be shown in the header of the \l MainView.
     */
    property string title

    /*!
      The contents of the header. If this is set, \l title will be ignored.
     */
    property Component headerContents: null

    /*!
      \internal
      Used to determine whether an Item is a Page.
     */
    property bool __isPage: true

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

    /*!
      The height of the header of the page.
     */
    readonly property real headerHeight: internal.headerHeight

    /*!
      The \l MainView that this Page is associated to. This property is automatically
       set and the Page will update the header of the main view. To disable this behavior,
       set mainView to null.
     */
    property var mainView: internal.getMainView()
    onMainViewChanged: internal.updateActivePage()

    Item {
        id: internal

        Connections {
            target: page
            onFlickableChanged: internal.updateFlickableMargins()
        }
        onHeaderHeightChanged: internal.updateFlickableMargins()

        function updateFlickableMargins() {
            if (flickable) {
                // Set-up the top-margin of the contents of the Flickable so that
                //  the contents is never hidden by the header:
                page.flickable.contentY = -headerHeight;
                page.flickable.topMargin = headerHeight;
            }
        }

        /*!
          One of the ancestors of this Page, before finding the MainView,
          is a Page. If true, do not set the current Page to the activePage of the
          MainView.
         */
        property bool hasAncestorPage: false;

        property real headerHeight: mainView && mainView.header ? mainView.header.height : 0

        function updateActivePage() {
            if (page.mainView && !hasAncestorPage) {
                page.mainView.activePage = page;
            }
        }

        function getMainView() {
            hasAncestorPage = false;
            var item = page.parent;
            var mainView = null;
            while (item && !mainView) {
                if (PageUtils.isPage(item)) hasAncestorPage = true;
                if (PageUtils.isMainView(item)) mainView = item;
                item = item.parent;
            }
            return mainView;
        }
    }
}
