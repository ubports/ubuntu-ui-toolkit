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

import QtQuick 2.0
import Ubuntu.Components 0.1

Template {
    TemplateSection {
        className: "Slider"

        TemplateFlow {
            title: i18n.tr("Standard")

            Slider {
                id: defaultSlider
                width: parent.width
            }
            Label {
                text: i18n.tr("value = %1").arg(defaultSlider.value)
            }
        }

        TemplateFlow {
            title: i18n.tr("Live Update")

            Slider {
                id: liveSlider
                width: parent.width
                live: true
            }
            Label {
                text: i18n.tr("value = %1").arg(liveSlider.value)
            }
        }

        TemplateFlow {
            title: i18n.tr("Range")

            Slider {
                id: rangeSlider
                width: parent.width
                minimumValue: -1.0
                maximumValue: 1.0
                function formatValue(v) { return Number(v.toFixed(2)).toLocaleString(Qt.locale()) }
            }
            Label {
                text: i18n.tr("value = %1").arg(rangeSlider.value)
            }
        }
    }
}
