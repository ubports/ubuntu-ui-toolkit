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
import QtQuick.Window 2.1
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.0

Window {
    id: main
    width: units.gu(200)
    height: units.gu(70)

    SplitView {
        id: layout
        anchors.fill: parent
        focus: true
        Keys.onSpacePressed: mainLayout.when = !mainLayout.when
        handleDelegate: Rectangle {
            radius: units.gu(1)
            anchors {
                fill: handle
                leftMargin: units.dp(1)
                rightMargin: units.dp(1)
                topMargin: handle.height / 2 - units.gu(3)
                bottomMargin: handle.height / 2 - units.gu(3)
            }
            color: UbuntuColors.graphite
            scale: handle.containsMouse || handle.pressed ? 1.6 : 1.0
            Behavior on scale { UbuntuNumberAnimation {} }
        }

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

        // TODO:!!!!
        Repeater {
            model: layout.activeLayout.columns
            Rectangle {
                color: UbuntuColors.red
                height: layout.height
                Label { text: parent.width + "/" + modelData.preferredWidth }
            }
        }

//        Rectangle {
//            color: UbuntuColors.red
//            height: parent.height
//            width: units.gu(90)
//            Label { text: parent.width }
//        }
//        Rectangle {
//            color: UbuntuColors.green
//            height: parent.height
//            Label { text: parent.width }
//            Component.onCompleted: print(SplitView.column)
//        }
//        Rectangle {
//            color: UbuntuColors.blue
//            height: parent.height
//            Label { text: parent.width }
//        }
//        Rectangle {
//            color: UbuntuColors.ash
//            height: parent.height
//            Label { text: parent.width }
//        }
//        Rectangle {
//            color: "pink"
//            height: parent.height
//            width: units.gu(30)
//            Label { text: parent.width }
//        }
    }
}
