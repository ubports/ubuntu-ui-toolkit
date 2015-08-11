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
import Ubuntu.PerformanceMetrics 1.0
import QtQuick.Window 2.0

/*! \internal */
// Documentation is in MainView.qdoc
MainViewBase {
    id: mainView

    /*!
      \qmlproperty bool automaticOrientation
     */
    property bool automaticOrientation: false
    property bool useDeprecatedToolbar: true

    /*!
      \internal
      Use default property to ensure children added do not draw over the toolbar.
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
                    
                    // move the whole contents up if the toolbar is locked and opened otherwise the toolbar will obscure part of the contents
                    bottomMargin: mainView.useDeprecatedToolbar &&
                                  toolbarLoader.item.locked && toolbarLoader.item.opened ?
                                      toolbarLoader.item.height + toolbarLoader.item.triggerSize : 0
                    // compensate so that the actual y is always 0
                    topMargin: -parent.y
                }
            }

            MouseArea {
                id: contentsArea
                anchors.fill: contents
                // This mouse area will be on top of the page contents, but
                // under the toolbar and header.
                // It is used for detecting interaction with the page contents
                // which can close the toolbar and take a tab bar out of selection mode.

                onPressed: {
                    mouse.accepted = false;
                    if (mainView.useDeprecatedToolbar) {
                        if (!toolbarLoader.item.locked) {
                            toolbarLoader.item.close();
                        }
                    }
                    if (headerItem.tabBar && !headerItem.tabBar.alwaysSelectionMode) {
                        headerItem.tabBar.selectionMode = false;
                    }
                }
                propagateComposedEvents: true
                enabled: mainView.useDeprecatedToolbar
            }
        }

        /*!
          Animate header and toolbar.
         */
        property bool animate: true

        Component {
            id: toolbarComponent
            Toolbar {
                parent: canvas
                onPressedChanged: {
                    if (!pressed) return;
                    if (headerItem.tabBar !== null) {
                        headerItem.tabBar.selectionMode = false;
                    }
                }
                animate: canvas.animate
                tools: internal.activePage ? internal.activePage.tools : null
            }
        }

        Loader {
            id: toolbarLoader
            sourceComponent: mainView.useDeprecatedToolbar ? toolbarComponent : null
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
            animate: canvas.animate
            dividerColor: Qt.darker(mainView.headerColor, 1.1)
            panelColor: Qt.lighter(mainView.headerColor, 1.1)

            title: internal.activePage ? internal.activePage.title : ""
            flickable: internal.activePage ? internal.activePage.flickable : null
            pageStack: internal.activePage ? internal.activePage.pageStack : null

            PageHeadConfiguration {
                id: headerConfig
                // for backwards compatibility with deprecated tools property
                actions: internal.activePage ?
                             getActionsFromTools(internal.activePage.tools) : null

                backAction: internal.activePage && internal.activePage.tools &&
                          internal.activePage.tools.hasOwnProperty("back") &&
                          internal.activePage.tools.back &&
                          internal.activePage.tools.back.hasOwnProperty("action") ?
                              internal.activePage.tools.back.action : null

                function getActionsFromTools(tools) {
                    if (!tools || !tools.hasOwnProperty("contents")) {
                        // tools is not of type ToolbarActions. Not supported.
                        return null;
                    }

                    var actionList = [];
                    for (var i in tools.contents) {
                        var item = tools.contents[i];
                        if (item && item.hasOwnProperty("action") && item.action !== null) {
                            var action = item.action;
                            if (action.hasOwnProperty("iconName") && action.hasOwnProperty("text")) {
                                // it is likely that the action is of type Action.
                                actionList.push(action);
                            }
                        }
                    }
                    return actionList;
                }
            }

            contents: internal.activePage ?
                          internal.activePage.__customHeaderContents : null

            // FIXME: This can be simplified a lot when we drop support for using
            //  the deprecated tools property.
            config: internal.activePage && internal.activePage.hasOwnProperty("head") &&
                    (internal.activePage.head.actions.length > 0 ||
                     internal.activePage.head.backAction !== null ||
                     internal.activePage.head.contents !== null ||
                     internal.activePage.head.sections.model !== undefined) ?
                        internal.activePage.head : headerConfig

            property Item tabBar: null
            Binding {
                target: headerItem
                property: "tabBar"
                value: headerItem.__styleInstance.__tabBar
                when: headerItem.__styleInstance &&
                      headerItem.__styleInstance.hasOwnProperty("__tabBar")
            }

            Connections {
                // no connections are made when target is null
                target: headerItem.tabBar
                onPressedChanged: {
                    if (mainView.useDeprecatedToolbar) {
                        if (headerItem.tabBar.pressed) {
                            if (!toolbarLoader.item.locked) toolbarLoader.item.close();
                        }
                    }
                }
            }

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

            useDeprecatedToolbar: mainView.useDeprecatedToolbar
        }

        Connections {
            target: Qt.application
            onActiveChanged: {
                if (Qt.application.active) {
                    canvas.animate = false;
                    headerItem.show();
                    if (headerItem.tabBar) {
                        headerItem.tabBar.selectionMode = true;
                    }
                    if (mainView.useDeprecatedToolbar) {
                        if (!toolbarLoader.item.locked) toolbarLoader.item.open();
                    }
                    canvas.animate = true;
                }
            }
        }
    }

    Object {
        id: internal

        // Even when using MainView 1.1, we still support Page 1.0.
        // we use PageTreeNode being the base class of all pages.
        property PageTreeNode activePage: isPage(mainView.activeLeafNode) ? mainView.activeLeafNode : null

        function isPage(item) {
            return item && item.hasOwnProperty("__isPageTreeNode") && item.__isPageTreeNode &&
                    item.hasOwnProperty("title") && item.hasOwnProperty("tools");
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
          \deprecated
          The toolbar that will be propagated to the children in the page tree node.
         */
        property Toolbar toolbar: toolbarLoader.item

        /*!
          \internal
          Tabs needs to know whether to use a TabBar or the new header.
         */
        property alias useDeprecatedToolbar: mainView.useDeprecatedToolbar

        /*!
          \internal
          The action manager that has the global context for the MainView's actions,
          and to which a local context can be added for each Page that has actions.actions.
         */
        property var actionManager: mainView.actionManager
    }
}
