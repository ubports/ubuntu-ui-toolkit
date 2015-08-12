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

Template {
    objectName: "proportionalShapesTemplate"

    TemplateSection {
        title: "ProportionalShape"
        className: "ProportionalShape"

        TemplateRow {
            title: i18n.tr("Sizes")
            titleWidth: units.gu(8)
            height: units.gu(14)

            ProportionalShape {
                objectName: "proportionalshape_size1"
                source: Image { source: "map_icon.png" }
                width: units.gu(6)
            }
            ProportionalShape {
                objectName: "proportionalshape_size2"
                source: Image { source: "map_icon.png" }
                width: units.gu(10)
            }
            ProportionalShape {
                objectName: "proportionalshape_size3"
                source: Image { source: "map_icon.png" }
                width: units.gu(14)
            }
        }
    }
}
