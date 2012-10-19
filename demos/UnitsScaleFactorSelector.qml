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

FocusScope {
    width: scaleFactorList.width
    height: scaleFactorList.height

    opacity: 0.8

    Rectangle {
        anchors.fill: parent
        color: "#cccccc"
    }

    property var scaleFactors: [1.0, 1.5, 2.0]

    ListView {
        id: scaleFactorList

        focus: true
        width: childrenRect.width
        height: units.gu(4)
        currentIndex: 0
        onCurrentIndexChanged: units.scaleFactor = scaleFactors[currentIndex]

        orientation: ListView.Horizontal
        model: scaleFactors
        delegate: ButtonWithForeground {
            width: units.gu(7)
            height: ListView.view.height
            text: modelData
            textSize: "small"
            textColor: "#757373"
            onClicked: scaleFactorList.currentIndex = index

            Rectangle {
                anchors.fill: parent
                z: -1
                color: "#e6dede"
                opacity: scaleFactorList.currentIndex == index ? 1.0 : 0.0
                Behavior on opacity {NumberAnimation {duration: 250; easing.type: Easing.OutQuart}}
            }
        }
    }
}
