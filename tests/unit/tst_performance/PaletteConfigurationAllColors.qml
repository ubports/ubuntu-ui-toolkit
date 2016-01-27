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
    width: 100
    height: 62

    property string newTheme

    theme: ThemeSettings {
        name: newTheme
        palette: Palette {
            normal {
                background: "#ABCDEF"
                backgroundText: "#0F0F0F"
                base: "#ABCDEF"
                baseText: "#0F0F0F"
                foreground: "#ABCDEF"
                foregroundText: "#0F0F0F"
                overlay: "#ABCDEF"
                overlayText: "#0F0F0F"
                field: "#ABCDEF"
                fieldText: "#0F0F0F"
                selection: "#ABCDEF"
            }
            selected {
                background: "#ABCDEF"
                backgroundText: "#0F0F0F"
                base: "#ABCDEF"
                baseText: "#0F0F0F"
                foreground: "#ABCDEF"
                foregroundText: "#0F0F0F"
                overlay: "#ABCDEF"
                overlayText: "#0F0F0F"
                field: "#ABCDEF"
                fieldText: "#0F0F0F"
                selection: "#ABCDEF"
            }
        }
    }
}
