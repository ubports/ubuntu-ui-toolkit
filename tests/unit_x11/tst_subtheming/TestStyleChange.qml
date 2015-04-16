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

StyledItem {
    width: units.gu(40)
    height: units.gu(71)
    objectName: "mainStyled"

    ThemeSettings {
        objectName: "Theme"
    }

    Component {
        id: dynamicComponent
        Item {
            property alias nestedStyled: innerItem
            StyledItem {
                id: innerItem
                anchors.fill: parent
            }
        }
    }

    Column {
        objectName: "simpleColumn"
        anchors.fill: parent
        StyledItem {
            objectName: "firstLevelStyled"
            width: parent.width
            height: units.gu(20)
            Item {
                anchors.fill: parent
                objectName: "simpleItem"
                StyledItem {
                    objectName: "secondLevelStyled"
                    anchors.fill: parent
                    Loader {
                        objectName: "thirdLevelLoader"
                        anchors.fill: parent
                        onItemChanged: if (item) item.nestedStyled.objectName = "thirdLevelLoaderStyled"
                        sourceComponent: dynamicComponent
                    }
                }
            }
        }
        Loader {
            objectName: "firstLevelLoader"
            width: parent.width
            height: units.gu(40)
            onItemChanged: if (item) item.nestedStyled.objectName = "firstLevelLoaderStyled"
            sourceComponent: dynamicComponent
        }
    }
}
