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
    property var tools: page ? page.tools : null
//    Binding on tools {
//        when: page
//        value: page.tools
//    }

//    onPageChanged: {
//        print(page.hasOwnProperty("tools") + " "+page.tools);
//        if (page.hasOwnProperty("tools")) rightItem.setTools(page.tools);
//        else rightItem.setTools(null);
//    }

    onToolsChanged: rightItem.setTools(toolbar.tools)
//    property Item backButton: leftItem.backButton
//    onBackButtonChanged: leftItem.setBackButton(toolbar.backButton)

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

        Item {
            id: leftItem
            anchors {
                left: parent.left
                top:parent.top
                bottom: parent.bottom
            }
            width: backButton ? backButton.width : 0

            property Item backButton: ChromeButton {
                parent: leftItem
                anchors.centerIn: parent
                icon: Qt.resolvedUrl("artwork/back.png")
                text: "Back"

                visible: toolbar.page && toolbar.page.hasOwnProperty("pageStack")
                         && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

                onClicked: toolbar.page.pageStack.pop()
            }

//            function setBackButton(newBackButton) {
//                if (backButton) backButton.parent = null;
//                backButton = newBackButton;
//                if (backButton) {
//                    backButton.parent = leftItem;
//                    backButton.anchors.centerIn = leftItem;
//                }
//            }
        }

        Row {
            id: rightItem
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width

            Repeater {
                model: rightItem.tools ? rightItem.tools : 0
                ChromeButton {
                    anchors {
                        bottom: parent.bottom
                        top: parent.top
                    }

//                    width: 100
//                    height: 40
//                    border.width: 1
//                    color: "yellow"
                    text: modelData.text
                    icon: modelData.icon ? modelData.icon : ""
                    onClicked: modelData.triggered()
                }
            }

            property var tools: ["first","second","third"]
            function setTools(newTools) {
                print("setting tools to "+newTools)
                // remove the old tools
                var i; // counter
//                for (i=0; i < rightItem.children.length; i++) {
//                    rightItem.children[i].parent = null;
//                }
                rightItem.tools = newTools;
                if (newTools === null) return;
                for (i=0; i < newTools.length; i++) {
                    print(newTools[i].text);
                }
            }

        }

//        Item {
//            id: rightItem
//            anchors {
//                right: parent.right
//                top: parent.top
//                bottom: parent.bottom
//            }
//            width: 300 //tools ? tools.width : 0
////            visible: tools !== null

//            visible: true

//            property var tools: null
//            function setTools(newTools) {
//                print("setting tools to "+newTools)
//                // remove the old tools
//                var i; // counter
//                for (i=0; i < rightItem.children.length; i++) {
//                    rightItem.children[i].parent = null;
//                }
//                rightItem.tools = newTools;
//                if (newTools === null) return;
//                for (i=0; i < newTools.length; i++) {
//                    print(newTools[i].text);
//                }
////                if (tools) tools.parent = null;
////                tools = newTools;
////                if (tools) {
////                    tools.parent = rightItem;
////                }
//            }

//            onToolsChanged: if (tools) print("tools.length = "+tools.length)

//            Repeater {
//                model: 3 //["aa", "bb", "cc"]// rightItem.tools ? rightItem.tools.length : 0
////                ChromeButton {
////                    text: modelData.text
////                }
//                Rectangle {
//                    color: "red"
//                    border.width: 1
//                    width: 50
//                    height: 60
//                    anchors.top: parent.top
//                    anchors.bottom: parent.bottom
//                    anchors.right: parent.right
//                    Component.onCompleted: print("completed rectangle "+modelData)
//                }
//            }
//        }
    }
}
