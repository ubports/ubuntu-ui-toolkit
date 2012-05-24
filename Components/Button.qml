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

/*!
    \qmlclass Button
    \inqmlmodule UbuntuUIToolkit
    \brief The Button class is DOCME

    \b{This component is under heavy development.}

    The Button class is part of the \l{UbuntuUIToolkit} module.
    A button can have text, an icon, or both.
*/
ColoredButton {
    id: button

    /*!
       \preliminary
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
    */
    property alias icon: icon.source

    /*!
       \preliminary
       The text to display in the button. If an icon was defined,
       the text will be shown next to the icon, otherwise it will
       be centered. Leave blank for an icon-only button.
    */
    property alias text: label.text

    /*!
      \preliminary
      The size of the text that is displayed in the button.
    */
    property alias textSize: label.fontSize

    /*!
      \preliminary
      The color of the text.
    */
    property alias textColor: label.color

    /*!
       \preliminary
       The width of the icon to display.
    */
    property alias iconWidth: icon.width

    /*!
       \preliminary
       The height of the icon to display.
    */
    property alias iconHeight: icon.height

    /*!
       \preliminary

       The position of the icon relative to the text.
       top, bottom, left or right.
       If only text or only an icon is defined, this
       property is ignored and the text or icon is
       centered horizontally and vertically in the button.
    */
    property string iconPosition: "left"

    /*!
       \preliminary
       DOCME
    */
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

    /*!
       \internal
    */
    function alignIconText() {
        if (button.icon=="") {
            // no icon.
            label.anchors.centerIn = button;
        } else if (button.text=="") {
            icon.anchors.centerIn = button;
        }
        else if (iconPosition=="top") {
            icon.anchors.top = button.top;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.top = icon.bottom;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconPosition=="bottom") {
            icon.anchors.bottom = button.bottom;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.bottom = icon.top;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconPosition=="right") {
            icon.anchors.right = button.right;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.right = icon.left;
            label.anchors.verticalCenter = button.verticalCenter;
        } else if (iconPosition=="left") {
            icon.anchors.left = button.left;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.left = icon.right;
            label.anchors.verticalCenter = button.verticalCenter;
        } // if textlocation
    } // alignIconText

    Component.onCompleted: alignIconText()
}
