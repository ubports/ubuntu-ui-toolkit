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

// TODO: documentation
GenericToolbar {
    id: toolbar

    /*!
      The page of which the tools must be displayed on the toolbar,
      and which can have a pageStack that is popped when the back button
      is clicked.
     */
    property Item page: null

    // TODO: automatically connect the tools to page.tools
    property Item tools
//    Binding on tools {
//        when: page
//        value: page.tools
//    }

//    onPageChanged: {
//        print(page.hasOwnProperty("tools") + " "+page.tools);
//        if (page.hasOwnProperty("tools")) buttonsGoHere.setTools(page.tools);
//        else buttonsGoHere.setTools(null);
//    }

    onToolsChanged: {
        buttonsGoHere.setTools(toolbar.tools);
    }

    Item {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
        }
        height: parent.height

        Rectangle { // TODO: make background themable?
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

        ChromeButton {
            id: backButton
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: units.gu(1) // TODO: make themable
            }
            icon: Qt.resolvedUrl("artwork/back.png")
            text: "Back"

            visible: toolbar.page && toolbar.page.hasOwnProperty("pageStack")
                     && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

            onClicked: toolbar.page.pageStack.pop()
        }

        Item {
            id: buttonsGoHere
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: tools ? tools.width : 0
            visible: tools !== null

            property Item tools: null
            function setTools(newTools) {
                print("setting tools to "+newTools)
                if (tools) tools.parent = null;
                tools = newTools;
                if (tools) {
                    tools.parent = buttonsGoHere;
                }
            }
        }
    }
}
