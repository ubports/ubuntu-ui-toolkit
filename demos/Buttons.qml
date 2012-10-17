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
    title: _("Buttons")

    Column {
        spacing: 30

        TemplateRow {
            title: _("Standard")

            Button {
                text: _("Call")
            }
        }

        TemplateRow {
            title: _("Disabled")

            Button {
                text: _("Call")
                enabled: false
            }
        }

        TemplateRow {
            title: _("Colors")

            Button {
                text: _("Call")
                color: "#37b301"
            }

            Button {
                text: _("Call")
                pressedColor: "#dd4f22"
            }

            Rectangle {
                id: darkBackground
                width: darkButton.width + 20
                height: darkButton.height + 20
                color: "#3a3c41"

                Button {
                    id: darkButton
                    text: _("Call")
                    color: "#dd4f22"
                    darkBorder: true
                    anchors.centerIn: parent
                }
            }
        }

        TemplateRow {
            title: _("Content")

            Button {
                text: _("Call")
            }

            Button {
                iconSource: "call_icon.png"
            }

            Button {
                text: _("Call")
                iconSource: "call_icon.png"
            }

            Button {
                text: _("Call")
                iconSource: "call_icon.png"
                iconPosition: "right"
            }
        }

        TemplateRow {
            title: _("Scalability")

            Button {
                text: _("Call")
            }

            Button {
                text: _("Call")
                width: 140
                height: 90
            }
        }
    }
}
