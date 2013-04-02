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
    \qmltype MainView
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief MainView is the root Item that should be used for all applications.
        It automatically adds a header and toolbar for its contents.

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
                    color: "blue"
                }

                tools: ToolbarActions {
                    Action {
                        text: "red"
                        onTriggered: rectangle.color = "red"
                    }
                    Action {
                        text: "green"
                        onTriggered: rectangle.color = "green"
                    }
                }
            }
        }
    \endqml
    This toolbar is hidden by default, but will be made visible when the user performs a bottom-edge-swipe gesture, and
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

    // FIXME: Make sure that the theming is only in the background, and the delegate
    //  should not occlude contents of the MainView. When making changes here, make
    //  sure that bug https://bugs.launchpad.net/manhattan/+bug/1124076 does not come back.
    Item {
        id: background
        Theming.ItemStyle.class: "mainview"
        anchors.fill: parent
    }

    // clip if the MainView is not fullscreen
    clip: true

    /*!
      MainView is active by default.
     */
    active: true

    /*!
      \internal
      Use default property to ensure children added do not draw over the toolbar.
     */
    default property alias contentsItem: contents.data
    Item {
        id: contents
        anchors.fill: parent
    }

    /*!
      \deprecated
      The tools of the main view's toolbar.
      This property is deprecated. Pages will now automatically update the toolbar when activated.
     */
    property ToolbarActions tools: null
    /*!
      \internal
      \deprecated
     */
    onToolsChanged: print("MainView.tools property was deprecated. "+
                          "Pages will automatically update the toolbar when activated. "+
                          "See CHANGES file, and use toolbar.tools instead when needed.");

    /*!
      The header of the MainView. Can be used to obtain the height of the header
      in \l Page to determine the area for the \l Page to fill.
     */
    header: headerItem
    Header {
        id: headerItem
    }

    toolbar: toolbarItem
    Toolbar {
        id: toolbarItem
    }

    /*! \internal */
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
}
