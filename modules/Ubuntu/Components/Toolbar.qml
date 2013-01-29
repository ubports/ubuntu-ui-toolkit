/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \internal
    \qmltype Toolbar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Application toolbar. This class is not exposed because it will
            be automatically added when a Page defines tools.
*/
GenericToolbar {
    id: toolbar
    Theming.ItemStyle.class: "toolbar"

    /*!
      The page of which the tools must be displayed on the toolbar,
      and which can have a pageStack that is popped when the back button
      is clicked.
     */
//    property Item page: null

    /*!
      \preliminary
      The list of \l Actions to be shown on the toolbar
     */
    property list<Action> tools

//    property variant something: [
//        "aaa",
//        "bbb"
//    ]

    /*!
      The action associated with the back button
     */
    property Action back: null
//    Action {
//        iconSource: Qt.resolvedUrl("artwork/back.png")
//        text: "Back"
//        visible: toolbar.page && toolbar.page.hasOwnProperty("pageStack")
//                 && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1
//        onTriggered: toolbar.page.pageStack.pop()
//    }

    Item {
        anchors.fill: parent

        Theming.ItemStyle.style: toolbar.Theming.ItemStyle.style
        Theming.ItemStyle.delegate: toolbar.Theming.ItemStyle.delegate

        MouseArea {
            // don't let mouse events go through the toolbar
            anchors.fill: parent
            // FIXME: Bug in qml? Without onClicked below, this MouseArea
            //      seems disabled.
            onClicked: { }
        }
    }

    Button {
        visible: toolbar.back && toolbar.back.visible
        Theming.ItemStyle.class: "toolbar-button"
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        // TODO: Make back buton themable in Page
        iconSource: toolbar.back ? toolbar.back.iconSource : ""
        text: toolbar.back ? toolbar.back.text : ""
        onClicked: toolbar.back.triggered()
    }

    Row {
        id: toolButtonsContainer
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: childrenRect.width

        // TODO: cross-fade buttons when tools property is updated?
//        property var tools: toolbar.page ? toolbar.page.tools : null

        Repeater {
            model: toolbar.tools.length > 0 ? toolbar.tools : 0 //toolButtonsContainer.tools ? toolButtonsContainer.tools : 0
            Button {
                Theming.ItemStyle.class: "toolbar-button"
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.text
                iconSource: modelData.iconSource ? modelData.iconSource : ""
                onClicked: modelData.triggered()
            }
        }
    }
}
