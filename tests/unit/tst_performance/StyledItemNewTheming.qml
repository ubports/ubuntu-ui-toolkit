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
import Ubuntu.Components 1.2

StyledItem {
    id: component1
    width: units.gu(50)
    height: units.gu(100)

    property string newTheme
    onNewThemeChanged: theme.name = newTheme

    Column {
        anchors.fill: parent
        StyledItem {
            id: component2
            width: units.gu(40)
            height: units.gu(80)

            StyledItem {
                id: component3
                width: units.gu(40)
                height: units.gu(80)
                Item {
                    anchors.fill: parent
                    StyledItem {
                        id: component4
                        width: units.gu(40)
                        height: units.gu(80)
                    }
                    Loader {
                        id: loader1
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
                StyledItem {
                    id: component5
                    anchors.fill: parent
                }
            }
        }

        Loader {
            id: loader2
            width: units.gu(30)
            height: units.gu(30)
            sourceComponent: dynamicItem
        }
    }
}
