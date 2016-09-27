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

//![0]
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3

Palette {
    normal: SuruDarkNormal {}
    disabled: SuruDarkNormal {
        Component.onCompleted: {
            // specific disabled colors
            var diff = {
                foreground: UbuntuColors.inkstone,
                field: UbuntuColors.inkstone,
                positiveText: UbuntuColors.porcelain,
                negativeText: UbuntuColors.porcelain,
                activityText: UbuntuColors.porcelain,
                focusText: UbuntuColors.porcelain
            };
            for (var p in normal) {
                // skip objectName and all change signals
                if (p == "objectName"|| p.indexOf("Changed") > 0) continue;
                disabled[p] = diff[p] || (
                    // if not specific, colors are 30% opaque normal
                    Qt.rgba(normal[p].r, normal[p].g, normal[p].b, normal[p].a * 0.5)
                );
            }
        }
    }

    // selected differs from normal in background, base, foreground
    selected: SuruDarkSelected {}

    // selected differs from normal in background, base, foreground
    selectedDisabled: SuruDarkSelected {
        Component.onCompleted: {
            var diff = {
                background: UbuntuColors.inkstone,
                base: UbuntuColors.inkstone,
                foreground: UbuntuColors.inkstone,
                positiveText: UbuntuColors.porcelain,
                negativeText: UbuntuColors.porcelain,
                activityText: UbuntuColors.porcelain,
                focusText: UbuntuColors.porcelain
            };
            for (var p in selected) {
                // skip objectName and all change signals
                if (p == "objectName"|| p.indexOf("Changed") > 0) continue;
                selectedDisabled[p] = diff[p] || (
                    // if not specific, colors are 30% opaque normal
                    Qt.rgba(selected[p].r, selected[p].g, selected[p].b, normal[p].a * 0.5)
                );
            }
        }
    }

    highlighted: SuruDarkNormal {
        background: UbuntuColors.slate
        base: UbuntuColors.slate
        baseText: UbuntuColors.silk
        foreground: UbuntuColors.slate
        raised: UbuntuColors.silk
        raisedText: UbuntuColors.inkstone
        raisedSecondaryText: UbuntuColors.ash
    }

    focused: SuruDarkNormal {
        background: Qt.rgba(UbuntuColors.blue.r, UbuntuColors.blue.g, UbuntuColors.blue.b, 0.4)
    }
}
//![0]
