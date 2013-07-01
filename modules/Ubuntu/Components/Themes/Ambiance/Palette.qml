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
import Ubuntu.Components.Themes 0.1

Palette {
    normal: PaletteValues {
        window: "#EEEEEE"
        windowText: "#7F888888"
        background: "#1A000000"
        backgroundText: "#888888"
        foreground: "#888888"
        foregroundText: "#F3F3E7"
        tooltip: "#FDFDFD"
        tooltipText: "#888888"
        field: "#1A000000" // FIXME: same as background
        fieldText: "#7F888888"
    }
    selected: PaletteValues {
        window: "#B2E6E6E6" // FIXME: not from design
        windowText: "#888888"
        foreground: "#DD4814"
        foregroundText: "#F3F3E7"
        field: "#FFFFFF"
        fieldText: "#888888"
    }
    disabled: PaletteValues {
    }
}
