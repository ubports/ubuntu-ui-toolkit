/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.0

Item {
    id: main
    width: units.gu(200)
    height: units.gu(70)

    SplitView {
        id: layout
        anchors.fill: parent

        layouts: [
            SplitViewLayout {
                id: mainLayout
                when: true
                ViewColumn {
                    preferredWidth: units.gu(40)
                    maximumWidth: units.gu(100)
                }
                ViewColumn {
                    fillWidth: true
                    minimumWidth: units.gu(10)
                    maximumWidth: units.gu(150)
                }
                ViewColumn {
                    preferredWidth: units.gu(50)
                }
                ViewColumn {
                    fillWidth: true
                    minimumWidth: units.gu(15)
                }
            },
            SplitViewLayout {
                when: !mainLayout.when
                ViewColumn {
                    preferredWidth: units.gu(10)
                    minimumWidth: units.gu(30)
                }
                ViewColumn {
                    fillWidth: true
                    minimumWidth: units.gu(40)
                }
            }
        ]

        Repeater {
            model: layout.activeLayout.columns
            Rectangle {
                objectName: "column" + index
                color: UbuntuColors.red
                height: layout.height
                Label { text: parent.width + "/" + modelData.preferredWidth }
            }
        }
    }
}
