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
    \qmltype Tabs
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Tabs class provides an environment where multible Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Examples:
    \qml
        Tabs {
            Tab {
                title: "tab 1"
                page: Text {
                    text: "This is the first tab."
                }
            }
            Tab {
                title: "tab 2"
                iconSource: "icon.png"
                page: Rectangle {
                    Text {
                        anchors.centerIn: parent
                        text: "Colorful tab."
                    }
                    color: "lightblue"
                }
            }
            Tab {
                title: "tab 3"
                page: Qt.resolvedUrl("MyCustomPage.qml")
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/

Item {
    // FIXME: see above
    Theming.ItemStyle.class: "tabs"

    /*!
      \preliminary
      The index of the currently selected tab.
      The first tab is 0, and -1 means that no tab is selected.
     */
    property int selectedTabIndex: -1

    onSelectedTabIndexChanged: print("selected tab "+selectedTabIndex)

    Component.onCompleted: {
        if (children.length > 0 && selectedTabIndex === -1) selectedTabIndex = 0;
    }

    // FIXME: The __pages, __pagesModel and pagesModel are a workaround for this bug:
    //  "theming: contentItem does work when it is a VisualItemModel"
    //  https://bugs.launchpad.net/tavastia/+bug/1080330
    //  The workaround does not break the regular TabsDelegate.
    /*! \internal */
    default property alias __pages: pagesModel.children
    /*!
      \internal
      required by SlidingTabsDelegate
     */    property alias __pagesModel: pagesModel
    VisualItemModel {
        id: pagesModel
    }
}
