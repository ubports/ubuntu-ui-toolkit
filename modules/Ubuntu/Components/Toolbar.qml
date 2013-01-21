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
//import Ubuntu.Components 0.1
//import Ubuntu.Components.ListItems 0.1 as ListItem

ChromeBar {
    id: toolbar

//    property Item tools: null
//    property alias tools: buttonsGoHere.data
//    onToolsChanged: print("tools changed to "+tools)

    property Item page

    onPageChanged: {
        print("new page = "+page);
        toolbar.setBarShown(false);
    }

    Item {
        id: contents
        anchors.left: parent.left
        anchors.right: parent.right
        //                height: chromeButtons.height + units.gu(2)
        height: units.gu(8)


        ChromeButton {
            id: backButton
            objectName: typeof index == 'undefined' ? "backButton" : "backButton" + index
            anchors.left: parent.left
            anchors.leftMargin: units.gu(1)
            anchors.top: parent.top
            icon: Qt.resolvedUrl("artwork/back.png")
            text: "Back"
            visible: toolbar.page && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

            onClicked: {
//                backButtonClicked()
                toolbar.page.pageStack.pop();
                toolbar.setBarShown(false);
            }

//            visible: showBackButton
        }

        Item {
            id: buttonsGoHere
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
//            width: theRow.width
//            width: contents.width - backButton.width
            width: childrenRect.width
//            height: childrenRect.height

            Rectangle {
                color: "pink"
                anchors.fill: parent
            }

//            Row {
//                id: theRow
//                anchors {
//                    right: parent.right
//                    top: parent.top
//                    bottom: parent.bottom
//                }
//                width: childrenRect.width

//                Rectangle {
//                    width: 100
//                    height: 100
//                    color: "pink"
//                }

                //                        ChromeButton {
                //                            icon: Qt.resolvedUrl("artwork/back.png")
                //                            text: "Lala"
                //                            onClicked: print("la!")
                //                        }

            }
        }

        //                Row {
        //                    id: chromeButtons
        //                    objectName: "viewerChromeButtons"
        //                    anchors.top: parent.top
        //                    anchors.right: parent.right
        //                    anchors.rightMargin: units.gu(1)
        //                    height: childrenRect.height

        //                    Repeater {
        //                        id: buttonsRepeater

        //                        ChromeButton {
        //                            id: chromeButton
        //                            text: model.label
        //                            icon: model.icon
        //                            objectName: model.name
        //                            enabled: (objectName !== "disabled")
        //                            anchors.top: parent.top
        //                            onClicked: buttonClicked(model.name, chromeButton)
        //                        }
        //                    }
        //                }
//    }
}
