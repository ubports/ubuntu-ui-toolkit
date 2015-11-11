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

/*!
    \qmltype MainView
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief MainView is the root Item that should be used for all applications.
        It automatically adds a header and toolbar for its contents and can
        rotate its content based on the device orientation.

    The simplest way to use a MainView is to include a single \l Page object
    inside the MainView:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            Page {
                title: "Simple page"
                Button {
                    anchors.centerIn: parent
                    text: "Push me"
                    width: units.gu(15)
                    onClicked: print("Click!")
                }
            }
        }
    \endqml
    It is not required to set the anchors of the \l Page as it will automatically fill its parent.
    The MainView has a header that automatically shows the title of the \l Page.

    Do not include multiple Pages directly inside the MainView, but use \l Tabs
    or \l PageStack inside MainView to navigate between several Pages.

    For the MainView to automatically rotate its content following the orientation
    of the device, set the \l automaticOrientation property to true.

    If the \l Page inside the MainView includes a Flickable with enough contents for scrolling,
    the header will automatically hide and show when the user scrolls up or down:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            Page {
                title: "Page with Flickable"

                Flickable {
                    anchors.fill: parent
                    contentHeight: column.height

                    Column {
                        id: column
                        Repeater {
                            model: 100
                            Label {
                                text: "line "+index
                            }
                        }
                    }
                }
            }
        }
    \endqml
    The same header behavior is automatic when using a ListView instead of a Flickable in the above
    example.

    The examples above show how to include a single \l Page inside a MainView, but more
    advanced application structures are possible using \l PageStack and \l Tabs.
*/
MainViewBase {
    id: mainView

    /*!
      \qmlproperty bool MainView::automaticOrientation
      \deprecated

      Sets whether the application will be automatically rotating when the
      device is.

      This property has no significance anymore as the shell rotates.
    */
    property bool automaticOrientation: false

    /*!
      \internal
      Use default property to ensure children added do not draw over the header.
     */
    default property alias contentsItem: contents.data
    Item {
        id: canvas
        anchors {
            fill: parent
            bottomMargin: mainView.anchorToKeyboard &&
                UbuntuApplication.inputMethod.visible ?
                    UbuntuApplication.inputMethod.keyboardRectangle.height : 0
        }

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
            dividerColor: Qt.darker(mainView.headerColor, 1.1)
            panelColor: Qt.lighter(mainView.headerColor, 1.1)

            title: internal.activePage ? internal.activePage.title : ""
            pageStack: internal.activePage ? internal.activePage.pageStack : null

            contents: internal.activePage &&
                      internal.activePage.hasOwnProperty("__customHeaderContents") ?
                          internal.activePage.__customHeaderContents : null

            Toolkit.PageHeadConfiguration {
                id: defaultConfig
                // Used when there is no active Page, or a Page 1.0 is used which
                // does not have a PageHeadConfiguration.
            }
            config: visible && internal.activePage &&
                    internal.activePage.hasOwnProperty("head") ?
                        internal.activePage.head : defaultConfig

            // don't show the application header if the page has its own header.
            visible: !(internal.activePage &&
                       internal.activePage.hasOwnProperty("header") &&
                       internal.activePage.header)

            height: visible ? implicitHeight : 0

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
        }

        Connections {
            target: Qt.application
            onActiveChanged: {
                if (Qt.application.active) {
                    if (!(headerItem.config &&
                          headerItem.config.hasOwnProperty("locked") &&
                          headerItem.config.locked)) {
                        headerItem.exposed = true;
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
