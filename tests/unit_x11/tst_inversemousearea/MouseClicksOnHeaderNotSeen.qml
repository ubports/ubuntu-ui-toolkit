/*
 * Copyright 2014 Canonical Ltd.
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

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Page {
        title: "Test"

        Rectangle {
            color: "blue"
            width: units.gu(30)
            height: units.gu(30)
            anchors.centerIn: parent
            clip: true

            Text {
                text: QuickUtils.consoleLog
                color: "white"
                onTextChanged: print("text=", text)
            }

            InverseMouseArea {
                id: ima
                objectName: "Test_IMA"
                anchors.fill: parent
                topmostItem: true

                onClicked: QuickUtils.log("IMA captured")
            }
        }
    }
}
