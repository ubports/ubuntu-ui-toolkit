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
import Ubuntu.Unity.Action 1.0 as UnityActions

/*!
    \qmltype MainView
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief MainView is the root Item that should be used for all applications.
        It automatically adds a header and toolbar for its contents and can
        rotate its content based on the device orientation.

    The simplest way to use a MainView is to include a \l Page object inside the MainView:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

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

    For the MainView to automatically rotate its content following the orientation
    of the device, set the \l automaticOrientation property to true.

    If the \l Page inside the MainView includes a Flickable with enough contents for scrolling, the header
    will automatically hide and show when the user scrolls up or down:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

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

    A toolbar can be added to the application by setting the tools property of the \l Page:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            Page {
                title: "Page title"
                Rectangle {
                    id: rectangle
                    anchors.centerIn: parent
                    width: units.gu(20)
                    height: units.gu(20)
                    color: UbuntuColors.coolGrey
                }

                tools: ToolbarActions {
                    ToolbarButton {
                        action: Action {
                            text: "red"
                            onTriggered: rectangle.color = UbuntuColors.orange
                        }
                    }
                    ToolbarButton {
                        action: Action {
                            text: "green"
                            onTriggered: rectangle.color = UbuntuColors.lightAubergine
                        }
                    }
                }
            }
        }
    \endqml
    The toolbar is hidden by default, but will be made visible when the user performs a bottom-edge-swipe gesture, and
    hidden when the user swipes it out, or when the active \l Page inside the MainView is changed.
    The examples above show how to include a single \l Page inside a MainView, but more advanced application
    structures are possible using \l PageStack and \l Tabs.
    See \l ToolbarActions for details on how to to control the behavior and contents of the toolbar.
*/
PageTreeNode {
    id: mainView

    /*!
      \preliminary
      The property holds the application's name, which must be the same as the
      desktop file's name.
      */
    property string applicationName

    // FIXME: Make sure that the theming is only in the background, and the style
    //  should not occlude contents of the MainView. When making changes here, make
    //  sure that bug https://bugs.launchpad.net/manhattan/+bug/1124076 does not come back.
    StyledItem {
        id: background
        anchors.fill: parent
        style: Theme.createStyleComponent("MainViewStyle.qml", background)
    }

    /*!
      MainView is active by default.
     */
    active: true

    /*!
      \preliminary
      Sets whether the application will be automatically rotating when the
      device is.

      The default value is false.

      \qmlproperty bool automaticOrientation
     */
    property alias automaticOrientation: canvas.automaticOrientation

    /*!
      \internal
      Use default property to ensure children added do not draw over the toolbar.
     */
    default property alias contentsItem: contents.data
    OrientationHelper {
        id: canvas

        automaticOrientation: false

        Item {
            id: contents
            anchors.fill: parent
        }

        /*!
          The header of the MainView. Can be used to obtain the height of the header
          in \l Page to determine the area for the \l Page to fill.
         */
        Header {
            id: headerItem
        }

        Toolbar {
            id: toolbarItem
        }
    }

    /*!
      Global actions. TODO: document
     */
//    property ActionList actions: null
    property alias actions: unityActionManager.actions

    Object {
        id: internal
        UnityActions.ActionManager {
            id: unityActionManager
        }

//        function updateActions() {
//            // TODO: clear global context actions
//            unityActionManager.actions = [];
//            print("updating actions?")
//            if (!mainView.actions) return;
//            print("updating actions" + mainView.actions.actions.length)
//            for (var i=0; i < mainView.actions.actions.length; i++) {
//                // Action is null on first actionsChanged when the list is initialized but the actions not yet.
//                if (mainView.actions.actions[i]) {
//                    print("action "+i+" = "+mainView.actions.actions[i]);
//                    // TODO: unityActionManager.actions.append(mainView.actions.actions[i]) when that works.
////                    unityActionManager.actions.append(mainView.actions.actions[i]);
//                    unityActionManager.globalContext.addAction(mainView.actions.actions[i]);
//                }
//            }
//        }

//        Connections {
//            target: mainView
//            onActionsChanged: internal.updateActions()
//            Component.onCompleted: internal.updateActions()
//        }
    }

    __propagated: QtObject {
        /*!
          \internal
          The header that will be propagated to the children in the page tree node.
          It will be used by the active \l Page to set the title.
         */
        property Header header: headerItem

        /*!
          \internal
          The toolbar that will be propagated to the children in the page tree node.
          It will be used by the active \l Page to set the toolbar actions.
         */
        property Toolbar toolbar: toolbarItem

        /*!
          \internal
          The action manager that has the global context for the MainView's actions,
          and to which a local context can be added for each Page that has actions.actions.
         */
        property var actionManager: unityActionManager
    }


    /*!
      \deprecated
      This property is DEPRECATED. Pages will now automatically update the toolbar when activated.
      See \l ToolbarItems for more information on how to set the tools of a \l Page.
     */
    property ToolbarActions tools: null
    /*!
      \internal
      \deprecated
     */
    onToolsChanged: print("MainView.tools property was DEPRECATED. "+
                          "Pages will automatically update the toolbar when activated. "+
                          "See CHANGES file, and use toolbar.tools instead when needed.");

    /*! \internal */
    onApplicationNameChanged: {
        if (applicationName !== "") {
            i18n.domain = applicationName;
        }
    }
}
