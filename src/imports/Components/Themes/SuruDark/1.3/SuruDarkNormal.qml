/*
 * Copyright 2016 Canonical Ltd.
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

PaletteValues {
    background: UbuntuColors.jet
    backgroundText: "#FFFFFF"
    backgroundSecondaryText: UbuntuColors.silk
    backgroundTertiaryText: UbuntuColors.ash
    base: UbuntuColors.graphite
    baseText: UbuntuColors.silk
    foreground: UbuntuColors.inkstone
    foregroundText: "#FFFFFF"
    raised: "#FFFFFF"
    raisedText: UbuntuColors.slate
    raisedSecondaryText: UbuntuColors.silk
    overlay: UbuntuColors.inkstone
    overlayText: "#FFFFFF"
    overlaySecondaryText: UbuntuColors.slate
    field: UbuntuColors.jet
    fieldText: "#FFFFFF"
    focus: UbuntuColors.orange
    focusText: "#FFFFFF"
    selection: Qt.rgba(UbuntuColors.blue.r, UbuntuColors.blue.g, UbuntuColors.blue.b, 0.4)
    selectionText: "#FFFFFF"
    positive: UbuntuColors.green
    positiveText: "#FFFFFF"
    negative: UbuntuColors.red
    negativeText: "#FFFFFF"
    activity: UbuntuColors.blue
    activityText: "#FFFFFF"
    position: "#00000000"
    positionText: UbuntuColors.blue
}
