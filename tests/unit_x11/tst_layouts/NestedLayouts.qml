/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Layouts 1.0

Item {
    id: root
    width: units.gu(40)
    height: units.gu(30)

    property string nestedLayout: ""

    Layouts {
        objectName: "layouts"
        id: layouts
        anchors.fill: parent
        layouts: [
            ConditionalLayout {
                name: "extra-large"
                when: layouts.width > units.gu(80)
                Item {
                    anchors.fill: parent
                    Item {
                        id: header
                        ItemLayout {
                            item: "item3"
                        }
                    }
                    Row {
                        anchors.fill: parent
                        anchors.topMargin: header.height
                        ItemLayout {
                            item: "item1"
                        }
                        ItemLayout {
                            item: "item2"
                        }
                    }
                }
            },
            ConditionalLayout {
                name: "simple"
                when: layouts.width <= units.gu(100)
                SimpleLayouts{
                    id: nested
                    anchors.fill: parent
                    onCurrentLayoutChanged: root.nestedLayout = nested.currentLayout
                }
            }
        ]

        // default layout
        DefaultLayout{
        }
    }
}
