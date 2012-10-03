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

import QtQuick 1.1
import Ubuntu.Components 0.1

Template {
    title: "Slider"

    Column {
        spacing: units.dp(30)

        TemplateRow {
            title: "Default"
            Slider {
                id: defaultSlider
            }
            TextCustom {
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
                text: '\'value\' is ' + defaultSlider.formatValue(defaultSlider.value)
            }
        }

        TemplateRow {
            title: "Live"
            Slider {
                id: liveSlider
                live: true
                minimumValue: -1.0
                maximumValue: 1.0
                value: 0.0
                function formatValue(v) { return v.toFixed(2) }
            }
            TextCustom {
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
                text: '\'value\' is ' + liveSlider.formatValue(liveSlider.value)
            }
        }
    }
}
