/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1

Template {
    title: i18n.tr("Resolution Independence")

    Column {
        spacing: units.gu(4)

        TemplateRow {
            title: i18n.tr("Grid Unit")

            Slider {
                id: slider

                function formatValue(v) { return Math.round(v) + " px" }
                minimumValue: 8
                maximumValue: 18
                Component.onCompleted: value = units.gridUnit
                onValueChanged: units.gridUnit = value
            }
        }

        TemplateRow {
            title: i18n.tr("1x1 GU")

            Rectangle {
                color: "darkgrey"
                width: units.gu(1)
                height: units.gu(1)
            }
        }

        TemplateRow {
            title: i18n.tr("4x4 GU")

            Rectangle {
                color: "darkgrey"
                width: units.gu(4)
                height: units.gu(4)
            }
        }

        TemplateRow {
            title: i18n.tr("10x10 GU")

            Rectangle {
                color: "darkgrey"
                width: units.gu(10)
                height: units.gu(10)
            }
        }
    }
}
