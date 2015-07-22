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
import Ubuntu.Components 1.3

Template {
    objectName: "slidersTemplate"

    TemplateSection {
        className: "Slider"
        spacing: units.gu(5)

        TemplateFlow {
            title: i18n.tr("Standard")

            Slider {
                id: defaultSlider
                objectName: "slider_standard"
                width: parent.width
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(defaultSlider.value)
                font.weight: Font.Light
            }
        }

        TemplateFlow {
            title: i18n.tr("Live Update")

            Slider {
                id: liveSlider
                objectName: "slider_live"
                width: parent.width
                live: true
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(liveSlider.value)
                font.weight: Font.Light
            }
        }

        TemplateFlow {
            title: i18n.tr("Range")

            Slider {
                id: rangeSlider
                objectName: "slider_range"
                width: parent.width
                minimumValue: -1.0
                maximumValue: 1.0
                function formatValue(v) { return Number(v.toFixed(2)).toLocaleString(Qt.locale()) }
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(rangeSlider.value)
                font.weight: Font.Light
            }
        }

        TemplateFlow {
            title: i18n.tr("Labels")

            Slider {
                id: longBubbleSlider
                objectName: "slider_long_bubble"
                width: parent.width
                value: 50
                minimumValue: 0
                maximumValue: 100
                function formatValue(v) {
                    if (v < maximumValue/4)
                        return i18n.tr("Small")
                    else if (v < maximumValue/1.5)
                        return i18n.tr("Medium")
                    else
                        return i18n.tr("Large")
                }
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(longBubbleSlider.value)
                font.weight: Font.Light
            }
        }
    }
}
