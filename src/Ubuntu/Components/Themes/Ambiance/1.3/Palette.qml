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
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3

Palette {
    normal {
        background: "#FFFFFF"
        backgroundText: UbuntuColors.charcoal
        backgroundSecondaryText: UbuntuColors.darkGrey
        backgroundTertiaryText: UbuntuColors.grey
        base: UbuntuColors.lightGrey
        baseText: UbuntuColors.darkGrey
        foreground: UbuntuColors.starDust
        foregroundText: UbuntuColors.charcoal
        raised: "#FFFFFF"
        raisedText: UbuntuColors.darkGrey
        raisedSecondaryText: UbuntuColors.lightGrey
        overlay: "#FFFFFF"
        overlayText: UbuntuColors.darkGrey
        overlaySecondaryText: UbuntuColors.lightGrey
        field: "#FFFFFF"
        fieldText: UbuntuColors.charcoal
        selection: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.2)
        selectionText: UbuntuColors.charcoal
        highlight: UbuntuColors.orange
        highlightText: "#FFFFFF"
        positive: UbuntuColors.green
        positiveText: "#FFFFFF"
        negative: UbuntuColors.red
        negativeText: "#FFFFFF"
        activity: UbuntuColors.blue
        activityText: "#FFFFFF"
    }
    selected {
        background: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.05)
        backgroundText: UbuntuColors.charcoal
        backgroundSecondaryText: UbuntuColors.darkGrey
        backgroundTertiaryText: UbuntuColors.grey
        base: UbuntuColors.lightGrey
        baseText: UbuntuColors.darkGrey
        foreground: UbuntuColors.starDust
        foregroundText: UbuntuColors.charcoal
        raised: "#FFFFFF"
        raisedText: UbuntuColors.darkGrey
        raisedSecondaryText: UbuntuColors.lightGrey
        overlay: "#FFFFFF"
        overlayText: UbuntuColors.darkGrey
        overlaySecondaryText: UbuntuColors.lightGrey
        field: "#FFFFFF"
        fieldText: UbuntuColors.charcoal
        selection: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.2)
        selectionText: UbuntuColors.charcoal
        highlight: UbuntuColors.orange
        highlightText: "#FFFFFF"
        positive: UbuntuColors.green
        positiveText: "#FFFFFF"
        negative: UbuntuColors.red
        negativeText: "#FFFFFF"
        activity: UbuntuColors.blue
        activityText: "#FFFFFF"
    }
}
