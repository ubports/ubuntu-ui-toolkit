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

import QtQuick 1.1

ColoredButton {
    id: button
    property alias icon: icon.source
    property alias text: label.text
    property alias iconWidth: icon.width
    property alias iconHeight: icon.height
    property alias fontSize: label.fontSize
    property alias fontColor: label.color


    /**
     * The location of the icon relative to the text.
     * top, bottom, left or right.
     * If only text or only an icon is defined, this
     * property is ignored and the text or icon is
     * centered horizontally and vertically in the button.
     */
    property string iconLocation: "left"

    signal clicked

    Image {
        id: icon
        fillMode: Image.PreserveAspectFit
        anchors.margins: 10
    }

    TextCustom {
        id: label
        anchors.margins: 10
        fontSize: "medium"
    }

    function alignIconText() {
        if (button.icon=="") {
            // no icon.
            label.anchors.centerIn = button;
        } else if (button.text=="") {
            icon.anchors.centerIn = button;
        }
        else if (iconLocation=="top") {
            icon.anchors.top = button.top;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.top = icon.bottom;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconLocation=="bottom") {
            icon.anchors.bottom = button.bottom;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.bottom = icon.top;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconLocation=="right") {
            //icon.height = button.height - 10;
            icon.anchors.right = button.right;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.right = icon.left;
            label.anchors.verticalCenter = button.verticalCenter;
        } else if (iconLocation=="left") {
            //icon.height = button.height - 10;
            icon.anchors.left = button.left;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.left = icon.right;
            label.anchors.verticalCenter = button.verticalCenter;
        } // if textlocation
    } // alignIconText

    Component.onCompleted: alignIconText()
}

