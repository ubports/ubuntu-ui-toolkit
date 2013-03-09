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
    objectName: "Switch"
    title: i18n.tr("Switches")

    Column {
        spacing: units.gu(4)
        width: units.gu(20)

        Row {
            height: units.gu(6)
            width: parent.width
            spacing: units.gu(2)
            Label {
                text: i18n.tr("Unchecked")
                width: units.gu(10)
            }
            CheckBox {
            }

            Switch {
            }
        }

        Row {
            spacing: units.gu(2)
            Label {
                text: i18n.tr("Checked")
                width: units.gu(10)
            }
            CheckBox {
                checked: true
            }
            Switch {
                checked: true
            }
        }

        Row {
            spacing: units.gu(2)
            Label {
                text: i18n.tr("Disabled")
                width: units.gu(10)
            }
            CheckBox {
                enabled: false
            }
            Switch {
                enabled: false
            }
            CheckBox {
                enabled: false
                checked: true
            }
            Switch {
                enabled: false
                checked: true
            }
        }
    }
}
