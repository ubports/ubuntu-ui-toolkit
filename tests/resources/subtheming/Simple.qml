/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3

StyledItem {
    id: root
    objectName: "Root"
    width: units.gu(50)
    height: units.gu(100)


    theme.name: "Ubuntu.Components.Themes.SuruGradient"

    Column {
        objectName: "C"
        anchors.fill: parent
        StyledItem {
            objectName: "firstLevelStyled"
            width: parent.width
            height: units.gu(20)
            theme: ThemeSettings {
                objectName: "Theme"
                name: parentTheme.name
            }

            Item {
                objectName: "I"
                anchors.fill: parent
                StyledItem {
                    objectName: "secondLevelStyled"
                    anchors.fill: parent
                }
            }
        }
    }


//    theme: ThemeSettings {
//        objectName: "RootTheme"
//        palette: Palette { normal.background: "pink" }
//    }

//    StyledItem {
//        id: t1
//        objectName: "T1"
//        anchors.fill: parent
//        anchors.bottomMargin: parent.height / 2
//        theme: ThemeSettings {
//            objectName:"T1Theme"
//            palette: Palette { normal.background: "red" }
//        }
//        Rectangle {
//            id: objectA
//            objectName: "A"
//            anchors.fill: parent;
//            anchors.bottomMargin: parent.height / 2
//            color: theme.palette.normal.background

//            StyledItem {
//                id: t3
//                objectName: "T3"
//                anchors.fill: parent
//                anchors.bottomMargin: parent.height / 2
//                theme: ThemeSettings {
//                    objectName: "T3Theme";
//                    palette: Palette { normal.background: "green" }
//                }
//                Rectangle {
//                    objectName: "C"
//                    anchors.fill: parent
//                    anchors.bottomMargin: parent.height / 2
//                    color: theme.palette.normal.background
//                }
//                Rectangle {
//                    objectName: "D"
//                    anchors.fill: parent
//                    anchors.topMargin: parent.height / 2
//                    color: Qt.lighter(theme.palette.normal.background, 2.0)
//                }
//            }
//            Item {
//                id: b
//                objectName: "B"
//                anchors.fill: parent;
//                anchors.topMargin: parent.height / 2
//                StyledItem {
//                    objectName: "T"
//                    anchors.fill: parent;
//                }
//            }
//        }
//    }
//    StyledItem {
//        id: t2
//        objectName: "T2"
//        anchors.fill: parent
//        anchors.topMargin: parent.height / 2
//        theme: ThemeSettings {
//            objectName:"T2Theme"
//            palette: Palette { normal.background: "blue" }
//        }
//        Rectangle {
//            id: e
//            objectName: "E"
//            anchors.fill: parent
//            anchors.topMargin: parent.height / 2
//            color: theme.palette.normal.background
//        }
//    }

    function traverseChildren(item, level) {
        var depthStr = "";
        for (var l = 0; l < level; l++) {
            depthStr += "-";
        }
        print(depthStr + ">", item.objectName, item.theme, (item.theme ? item.theme.parentTheme : "()"));
        for (var i = 0; i < item.children.length; i++) {
            traverseChildren(item.children[i], level + 1);
        }
    }

//    function usecase1() {
//        print("usecase 1: REPARENT A to T2");
//        var prevParent = objectA.parent;
//        objectA.parent = t2;
//        traverseChildren(root, 1)
//        objectA.parent = prevParent;
//    }
//    function usecase2() {
//        print("usecase 2: REPARENT T3 to T2");
//        var prevParent = t3.parent;
//        t3.parent = t2;
//        traverseChildren(root, 1)
//        t3.parent = prevParent;
//    }
//    function usecase3() {
//        print("usecase 3: REPARENT E to B");
//        var prevParent = e.parent;
//        e.parent = b;
//        traverseChildren(root, 1)
//        e.parent = prevParent;
//    }
//    function usecase4() {
//        print("usecase 4: REPARENT B to T1");
//        var prevParent = b.parent;
//        b.parent = t1;
//        traverseChildren(root, 1)
//        b.parent = prevParent;
//    }

//    Timer {
//        id: reparentTimer
//        repeat: false
//        interval: 5000
//        onTriggered: usecase4()
//    }

    Component.onCompleted: {
        traverseChildren(root, 1)
//        reparentTimer.start();
//        print(t1.theme.name);
    }
}
