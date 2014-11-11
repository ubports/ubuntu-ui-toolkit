/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components.Themes.Ambiance 1.1

Grid {
    width: 800
    height: 600
    rows: 16
    columns: 16
    Repeater {
        model: 16*16
        Item {
            id: thisItem
            property Item item: thisItem
            property real minimumValue: 0.0
            property real maximumValue: 1.0
            property bool live
            function formatValue(v){return v}
            property var __internals: MouseArea {
                property bool isPressed
                property bool liveValue
            }
            SliderStyle {
                property Item styledItem: thisItem
            }
        }
    }
}
