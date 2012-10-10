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
    title: gallery.tr("Buttons")

    Column {
        spacing: 30

        TemplateRow {
            title: gallery.tr("Standard") + emptyString

            Button {
                text: gallery.tr("Call") + emptyString
            }
        }

        TemplateRow {
            title: tr("Disabled")

            Button {
                text: gallery.tr("Call")
                enabled: false
            }
        }

        TemplateRow {
            title: gallery.tr("Colors")

            Button {
                text: gallery.tr("Call")
//                text: gallery.tr("Call", "telephony-app") // context
                color: "#37b301"
            }

            Button {
                text: gallery.tr("Call")
//                text: gallery.tr("Call", "default") // context
                pressedColor: "#dd4f22"
            }

            Rectangle {
                id: darkBackground
                width: darkButton.width + 20
                height: darkButton.height + 20
                color: "#3a3c41"

                Button {
                    id: darkButton
                    text: gallery.tr("Call")
                    color: "#dd4f22"
                    darkBorder: true
                    anchors.centerIn: parent
                }
            }
        }

        TemplateRow {
            title: gallery.tr("Content")

            Button {
                text: gallery.tr("Call")
            }

            Button {
                iconSource: "call_icon.png"
            }

            Button {
                text: gallery.tr("Call")
                iconSource: "call_icon.png"
            }

            Button {
                text: gallery.tr("Call")
                iconSource: "call_icon.png"
                iconPosition: "right"
            }
        }

        TemplateRow {
            title: gallery.tr("Scalability")

            Button {
                text: gallery.tr("Call")
            }

            Button {
                text: gallery.tr("Call")
                width: 140
                height: 90
            }
        }
    }

//    Component.onCompleted: {
//        i18n.domain = "gallery"
//        i18n.localeDir = "/usr/share/locale";
//    }
}
