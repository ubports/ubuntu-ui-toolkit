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

//![0]
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3

Palette {
    normal {
        background: "#221E1C"
        backgroundText: "#5D5D5D78"
        base: "#19000000"
        baseText: "#FFFFFF"
        foreground: "#888888"
        foregroundText: "#FFFFFF"
        overlay: "#F2F2F2"
        overlayText: "#888888"
        field: "#19000000"
        fieldText: "#7F7F7F7F"
    }
    selected {
        background: "#88D6D6D6"
        backgroundText: "#FFFFFF"
        selection: Qt.rgba(UbuntuColors.blue.r, UbuntuColors.blue.g, UbuntuColors.blue.b, 0.2)
        foreground: UbuntuColors.orange
        foregroundText: UbuntuColors.darkGrey
        field: "#FFFFFF"
        fieldText: "#888888"
    }
}
//![0]
