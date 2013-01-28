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

    /*!
      The page of which the tools must be displayed on the toolbar,
      and which can have a pageStack that is popped when the back button
      is clicked.
     */
    property Item page: null

    Rectangle { // TODO: make background themable
        anchors.fill: parent
        color: "white"

        MouseArea {
            // don't let mouse events go through the toolbar
            anchors.fill: parent
            // FIXME: Bug in qml? Without onClicked below, this MouseArea
            //      seems disabled.
            onClicked: { }
        }
    }

    Button {
        Theming.ItemStyle.class: "toolbar-button"
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        // TODO: Make back buton themable in Page
        iconSource: Qt.resolvedUrl("artwork/back.png")
        text: "Back"

        visible: toolbar.page && toolbar.page.hasOwnProperty("pageStack")
                 && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

        onClicked: toolbar.page.pageStack.pop()
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
        property var tools: toolbar.page ? toolbar.page.tools : null

        Repeater {
            model: toolButtonsContainer.tools ? toolButtonsContainer.tools : 0
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
