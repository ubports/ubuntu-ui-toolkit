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
    title: i18n.tr("Slider")

    Column {
        spacing: units.gu(4)

        TemplateRow {
            title: i18n.tr("Default")
            Slider {
                id: defaultSlider
            }
            TextCustom {
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
                text: i18n.tr("\'value\' is %1").arg(defaultSlider.formatValue(defaultSlider.value))
            }
        }

        TemplateRow {
            title: i18n.tr("Live")
            Slider {
                id: liveSlider
                live: true
                minimumValue: -1.0
                maximumValue: 1.0
                value: 0.0
                function formatValue(v) { return Number(v.toFixed(2)).toLocaleString(Qt.locale()) }
            }
            TextCustom {
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
                text: i18n.tr("\'value\' is %1").arg(liveSlider.formatValue(liveSlider.value))
            }
        }
    }
}
