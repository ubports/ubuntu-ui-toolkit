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

import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.Themes 0.1

Palette {
    normal: PaletteValues {
        background: "#EDEDED"
        backgroundText: "#81888888"
        base: Qt.rgba(0, 0, 0, 0.1)
        baseText: UbuntuColors.lightGrey
        foreground: UbuntuColors.lightGrey
        foregroundText: "#FFFFFF"
        overlay: "#FDFDFD"
        overlayText: UbuntuColors.lightGrey
        field: "#FAFAFA"
        fieldText: UbuntuColors.darkGrey
    }
    selected: PaletteValues {
        background: Qt.rgba(0, 0, 0, 0.05)
        backgroundText: UbuntuColors.darkGrey
        selection: foreground // unused
        foreground: Qt.rgba(UbuntuColors.blue.r, UbuntuColors.blue.g, UbuntuColors.blue.b, 0.2)
        foregroundText: UbuntuColors.darkGrey
        field: "#FFFFFF"
        fieldText: UbuntuColors.darkGrey
    }
}
