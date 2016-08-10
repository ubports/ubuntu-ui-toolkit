/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Label {
        id: clickedLabel
        objectName: "clickedLabel"
        text: "No element clicked."
    }

    Flickable {
        anchors {
            fill: parent
            topMargin: clickedLabel.height
            // It can't be at the bottom, or the toolbar will be opened
            // when we try to click it.
            bottomMargin: units.gu(10)
        }
        objectName: 'flickable'
        height: units.gu(60)
        contentHeight: bottomButton.y + bottomButton.height

        Button {
            id: topButton
            objectName: 'topButton'
            text: 'Top button'
            onClicked: clickedLabel.text = objectName
        }
        Rectangle {
            id: emptyRectangle
            height: units.gu(80)
            anchors.top: topButton.bottom
        }
        Button {
            id: bottomButton
            objectName: 'bottomButton'
            text: 'Bottom button'
            onClicked: clickedLabel.text = objectName
            anchors.top: emptyRectangle.bottom
        }
    }
}
