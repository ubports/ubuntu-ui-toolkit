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
    objectName: "iconsTemplate"

    TemplateSection {
        className: "Icon"

        TemplateRow {
            title: i18n.tr("Scaling")
            spacing: units.gu(2)

            Icon {
                name: "call-start"
                width: 16
                height: 16
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: 96
                height: 96
            }
        }

        TemplateRow {
            title: i18n.tr("Colorization")
            spacing: units.gu(2)

            Icon {
                name: "computer-symbolic"
                width: 24
                height: 24
            }

            Icon {
                name: "computer-symbolic"
                color: UbuntuColors.orange
                keyColor: "#bebebe"
                width: 24
                height: 24
            }

            Icon {
                name: "computer-symbolic"
                color: UbuntuColors.lightAubergine
                keyColor: "#bebebe"
                width: 24
                height: 24
            }
        }
    }
}
