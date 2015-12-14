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
    normal: SuruDarkNormal {}
    inactive: SuruDarkNormal {
        field: UbuntuColors.cinder
        // inactive is 30% transparent normal
        Component.onCompleted: {
            for (var p in normal) {
                // skip objectName and all change signals
                if (p == "objectName" || p == "field" || p.indexOf("Changed") > 0) continue;
                inactive[p] = Qt.rgba(normal[p].r, normal[p].g, normal[p].b, 0.3);
            }
        }
    }

    // selected differs from normal in background, base, foreground
    selected: SuruDarkNormal {
        background: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.4)
        base: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.4)
        foreground: Qt.rgba(UbuntuColors.orange.r, UbuntuColors.orange.g, UbuntuColors.orange.b, 0.4)
    }

    // selected differs from normal in background, base, foreground
    selectedInactive: SuruDarkNormal {
        background: UbuntuColors.cinder
        base: UbuntuColors.cinder
        foreground: UbuntuColors.cinder
    }
}
//![0]
