/*
 * Copyright 2014 Canonical Ltd.
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

StyledItem {
    id: component1
    objectName: "TopItem"
    width: units.gu(50)
    height: units.gu(100)

    Column {
        objectName: "Column"
        anchors.fill: parent
        TextField {
            objectName: "OuterText"
            property string styleName: theme.name
            onStyleNameChanged: print(objectName, styleName)
        }
        StyledItem {
            objectName: "SuruDarkStyled"
            width: parent.width
            height: units.gu(10)
            property StyleSet theme: StyleSet {
                name: "Ubuntu.Components.Themes.SuruDark"
            }
            TextField {
                objectName: "InnerText"
                property string styleName: theme.name
                onStyleNameChanged: print(objectName, styleName)
            }
        }
    }


//    StyleSet {
//        id: customSet
//        name: "Ubuntu.Components.Themes.Ambiance"
//    }

//    Column {
//        anchors.fill: parent
//        TextField { text: "Press me" }
//        StyledItem {
//            id: component2
//            width: units.gu(40)
//            height: units.gu(80)

//            StyledItem {
//                id: component3
//                width: units.gu(40)
//                height: units.gu(80)

//                styleSet: StyleSet {
//                    name: "Ubuntu.Components.Themes.SuruDark"
//                }
//                Column {
//                    TextField { text: "Press me" }
//                    StyledItem {
//                        id: component4
//                        width: units.gu(40)
//                        height: units.gu(80)
//                    }
//                    Loader {
//                        id: loader1
//                        width: units.gu(30)
//                        height: units.gu(30)
//                    }
//                }
//            }
//        }
//        Component {
//            id: dynamicItem
//            Item {
//                property alias styleSet: component5.styleSet
//                StyledItem {
//                    id: component5
//                    anchors.fill: parent
//                }
//            }
//        }

//        Loader {
//            id: loader2
//            width: units.gu(30)
//            height: units.gu(30)
//        }
//    }

//    function printComponents(message) {
//        print("----Component styleSets:", message)
//        print("component1", component1.styleSet.name);
//        print("component2", component2.styleSet.name);
//        print("component3", component3.styleSet.name);
//        print("component4", component4.styleSet.name);
//        if (loader1.item) {
//            print("loader1.item", loader1.item.styleSet.name);
//        }
//        if (loader2.item) {
//            print("loader2.item", loader2.item.styleSet.name);
//        }
//        print("------------------------")
//    }

//    Component.onCompleted: {
//        printComponents("default");
////        // reparent component4
////        component4.parent = component2;
////        printComponents("component4 reparented to component2");
////        // load a dynamic and nested StyledItem
////        loader1.sourceComponent = dynamicItem;
////        printComponents("loader1 set");
////        loader2.sourceComponent = dynamicItem;
////        printComponents("loader2 set");

////        component2.styleSet = customSet;
////        printComponents("component2 customSet");
////        Theme.name = "Ubuntu.Components.Themes.SuruGradient";
////        component1.styleSet = customSet;
////        printComponents("system theme changed");
//    }
}
