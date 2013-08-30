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
        className: "Button"

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                objectName: "button_text"
                text: i18n.tr("Call")
            }
        }

        TemplateRow {
            title: i18n.tr("Color")

            Button {
                objectName: "button_color"
                text: i18n.tr("Call")
                gradient: UbuntuColors.greyGradient
            }
        }

        TemplateRow {
            title: i18n.tr("Icon")

            Button {
                objectName: "button_iconsource"
                iconSource: "call.png"
            }
        }

        TemplateRow {
            title: i18n.tr("Icon+Text")

            Button {
                objectName: "button_iconsource_right_text"
                width: units.gu(10)
                text: i18n.tr("Call")
                iconSource: "call.png"
                iconPosition: "right"
            }

            Button {
                objectName: "button_iconsource_left_text"
                width: units.gu(10)
                text: i18n.tr("Call")
                iconSource: "call.png"
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Button {
                objectName: "button_text_disabled"
                text: i18n.tr("Call")
                enabled: false
            }
        }
    }
}
