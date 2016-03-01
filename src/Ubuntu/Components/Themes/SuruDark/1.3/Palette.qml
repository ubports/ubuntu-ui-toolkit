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
        field: UbuntuColors.inkstone
        // inactive is 30% transparent normal
        Component.onCompleted: {
            for (var p in normal) {
                // skip objectName and all change signals
                if (p == "objectName"|| p.indexOf("Changed") > 0
                         || p == "field"  || p == "foreground" ) continue;
                disabled[p] = Qt.rgba(normal[p].r, normal[p].g, normal[p].b, 0.3);
            }
        }
    }

    // selected differs from normal in background, base, foreground
    selected: SuruDarkSelected {}

    // selected differs from normal in background, base, foreground
    selectedDisabled: SuruDarkSelected {
        background: UbuntuColors.inkstone
        base: UbuntuColors.inkstone
        foreground: UbuntuColors.inkstone
        // inactive is 30% transparent normal
        Component.onCompleted: {
            for (var p in selected) {
                // skip objectName and all change signals
                if (p == "objectName"|| p.indexOf("Changed") > 0
                         || p == "field"  || p == "foreground" ) continue;
                selectedDisabled[p] = Qt.rgba(selected[p].r, selected[p].g, selected[p].b, 0.3);
            }
        }
    }

    highlighted: SuruDarkNormal {
        background: UbuntuColors.slate
        base: UbuntuColors.graphite
        foreground: UbuntuColors.slate
    }
}
//![0]
