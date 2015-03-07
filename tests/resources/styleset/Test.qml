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
    onStyleSetChanged: print("component1", styleSet.name)

    StyleSet {
        id: ambiance
        name: "Ubuntu.Components.Themes.Ambiance"
    }
    StyleSet {
        id: suru
        name: "Ubuntu.Components.Themes.SuruDark"
    }
    StyleSet {
        id: gradient
        name: "Ubuntu.Components.Themes.SuruGradient"
    }


    Column {
        anchors.fill: parent
        Button {
            text: "component1 - Ambiance"
            height: units.gu(4)
            onClicked: component1.styleSet = ambiance
        }
        Button {
            text: "component1 - Suru"
            height: units.gu(4)
            onClicked: component1.styleSet = suru
        }
        Button {
            text: "component1 - Gradient"
            height: units.gu(4)
            onClicked: component1.styleSet = gradient
        }
        Button {
            text: "component2 - Ambiance"
            height: units.gu(4)
            onClicked: component2.styleSet = ambiance
        }
        Button {
            text: "component2 - Suru"
            height: units.gu(4)
            onClicked: component2.styleSet = suru
        }
        Button {
            text: "component2 - Gradient"
            height: units.gu(4)
            onClicked: component2.styleSet = gradient
        }
        Button {
            text: text2.parent == column ? "text2^ = column" : "text2^ = component2"
            height: units.gu(4)
            onClicked: {
                if (text2.parent == column) {
                    text2.parent = component2;
                } else {
                    text2.parent = column;
                }
            }
        }


        TextField {
            id: text1
            text: "Press me"
            onStyleSetChanged: print("text1", styleSet.name)
        }
        StyledItem {
            id: component2
            width: parent.width
            height: units.gu(80)
            onStyleSetChanged: print("component2", styleSet.name)

            StyledItem {
                id: component3
                onStyleSetChanged: print("component3", styleSet.name)
                anchors.fill: parent
                styleSet: StyleSet {
                    name: "Ubuntu.Components.Themes.SuruDark"
                }
                Column {
                    id: column
                    TextField {
                        id: text2
                        text: "Press me"
                        onStyleSetChanged: print("text2", styleSet.name)
                    }
                    StyledItem {
                        id: component4
                        onStyleSetChanged: print("component4", styleSet.name)
                        width: units.gu(40)
                        height: units.gu(20)
                    }
                    Loader {
                        id: loader1
                        onItemChanged: {
                            item.objectName = "loader1.item";
                        }
                        width: units.gu(30)
                        height: units.gu(30)
                        sourceComponent: dynamicItem
                    }
                }
            }
        }
        Component {
            id: dynamicItem
            Item {
                property alias styleSet: component5.styleSet
                onStyleSetChanged: print(objectName, styleSet.name)
                StyledItem {
                    id: component5
                    anchors.fill: parent
                }
            }
        }

        Loader {
            id: loader2
            onItemChanged: {
                item.objectName = "loader1.item";
            }
            width: units.gu(30)
            height: units.gu(30)
            sourceComponent: dynamicItem
        }
    }

    function printComponents(message) {
        print("----Component styleSets:", message)
        print("component1", component1.styleSet.name);
        print("component2", component2.styleSet.name);
        print("component3", component3.styleSet.name);
        print("text1", text1.styleSet.name);
        print("text2", text1.styleSet.name);
        print("component4", component4.styleSet.name);
        if (loader1.item) {
            print("loader1.item", loader1.item.styleSet.name);
        }
        if (loader2.item) {
            print("loader2.item", loader2.item.styleSet.name);
        }
        print("------------------------\n")
    }
}
