/*
 * Copyright 2012-2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3 as Toolkit
import Ubuntu.PerformanceMetrics 1.0
import QtQuick.Window 2.2

/*! \internal */
// Documentation is in MainView.qdoc
MainViewBase {
    id: mainView

    property alias automaticOrientation: canvas.automaticOrientation

    /*!
      \internal
      Use default property to ensure children added do not draw over the header.
     */
    default property alias contentsItem: contents.data
    OrientationHelper {
        id: canvas

        automaticOrientation: false
        anchorToKeyboard: mainView.anchorToKeyboard

        // clip the contents so that it does not overlap the header
        Item {
            id: contentsClipper
            anchors {
                left: parent.left
                right: parent.right
                top: headerItem.bottom
                bottom: parent.bottom
            }
            // only clip when necessary
            // ListView headers may be positioned at the top, independent from
            // flickable.contentY, so do not clip depending on activePage.flickable.contentY.
            clip: headerItem.bottomY > 0 && internal.activePage && internal.activePage.flickable

            Item {
                id: contents
                anchors {
                    fill: parent
                    
                    // compensate so that the actual y is always 0
                    topMargin: -parent.y
                }
            }
        }

        /*!
          The header of the MainView. Can be used to obtain the height of the header
          in \l Page to determine the area for the \l Page to fill.
         */
        AppHeader {
            // This objectName is used in the MainView autopilot custom proxy object
            // in order to select the application header.
            // Also used in tst_header_locked_visible.qml.
            objectName: "MainView_Header"
            id: headerItem
            property real bottomY: headerItem.y + headerItem.height
            animate: true
            dividerColor: Qt.darker(mainView.headerColor, 1.1)
            panelColor: Qt.lighter(mainView.headerColor, 1.1)

            title: internal.activePage ? internal.activePage.title : ""
            flickable: internal.activePage ? internal.activePage.flickable : null
            pageStack: internal.activePage ? internal.activePage.pageStack : null

            contents: internal.activePage &&
                      internal.activePage.hasOwnProperty("__customHeaderContents") ?
                          internal.activePage.__customHeaderContents : null

            Toolkit.PageHeadConfiguration {
                id: defaultConfig
                // Used when there is no active Page, or a Page 1.0 is used which
                // does not have a PageHeadConfiguration.
            }
            config: internal.activePage && internal.activePage.hasOwnProperty("head") ?
                        internal.activePage.head : defaultConfig

            // 'window' is defined by QML between startup and showing on the screen.
            // There is no signal for when it becomes available and re-declaring it is not safe.
            property bool windowActive: typeof window != 'undefined'
            onWindowActiveChanged: {
                window.title = headerItem.title
            }

            Connections {
                target: headerItem
                onTitleChanged: {
                    if (headerItem.windowActive)
                        window.title = headerItem.title
                }
            }

            // Use of the deprecated toolbar is no longer supported in MainView 1.2.
            useDeprecatedToolbar: false
        }

        Connections {
            target: Qt.application
            onActiveChanged: {
                if (Qt.application.active) {
                    if (!(headerItem.config &&
                          headerItem.config.hasOwnProperty("locked") &&
                          headerItem.config.locked)) {
                        headerItem.animate = false;
                        headerItem.show();
                        headerItem.animate = true;
                    }
                }
            }
        }
    }

    Toolkit.Object {
        id: internal

        // Even when using MainView 1.1, we still support Page 1.0.
        // PageBase (=Page 1.0) is the superclass of Page 1.1.
        property Item activePage: isPage(mainView.activeLeafNode) ? mainView.activeLeafNode : null

        function isPage(item) {
            return item && item.hasOwnProperty("__isPageTreeNode") &&
                    item.__isPageTreeNode &&
                    item.hasOwnProperty("title") &&
                    item.hasOwnProperty("flickable") &&
                    item.hasOwnProperty("active") &&
                    item.hasOwnProperty("pageStack")
        }
    }

    __propagated: QtObject {
        /*!
          \internal
          The header that will be propagated to the children in the page tree node.
          It is used by Tabs to bind header's tabsModel.
         */
        property AppHeader header: headerItem

        /*!
          \internal
          The action manager that has the global context for the MainView's actions,
          and to which a local context can be added for each Page that has actions.actions.
         */
        property var actionManager: mainView.actionManager

        /*!
          \internal
          Used by PageStack. This property only exists in MainView 1.2 and later.
         */
        readonly property bool animateHeader: false

        // FIXME: Currently disabled to prevent bug 1461729
//        readonly property bool animateHeader: headerItem.__styleInstance &&
//                                              headerItem.__styleInstance.hasOwnProperty("animateIn") &&
//                                              headerItem.__styleInstance.hasOwnProperty("animateOut") &&
//                                              headerItem.__styleInstance.hasOwnProperty("animateInFinished") &&
//                                              headerItem.__styleInstance.hasOwnProperty("animateOutFinished")
    }
}
