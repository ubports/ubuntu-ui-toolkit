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

ChromeBar {
    id: toolbar

    /*!
      The page of which the tools must be displayed on the toolbar,
      and which can have a pageStack that is popped when the back button
      is clicked.
     */
    property Item page: null

    onPageChanged: {
        print("new page = "+page);
        print("new tools = "+page.tools);
//        toolbar.active = false;
//        toolbar.tools = page.tools
        buttonsGoHere.setTools(page.tools);
    }

    Item {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
        }
        height: units.gu(8)

        ChromeButton {
            id: backButton
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: units.gu(1) // TODO: make themable
            }
            icon: Qt.resolvedUrl("artwork/back.png")
            text: "Back"

            visible: toolbar.page && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

            onClicked: toolbar.page.pageStack.pop()
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


            function setTools(newtools) {
                print("setting tools to "+newtools);
            }

//            Rectangle {
//                color: "pink"
//                anchors.fill: parent
//            }

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
