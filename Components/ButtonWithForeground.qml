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
    \qmlclass ButtonWithForeground
    \inqmlmodule UbuntuUIToolkit
    \brief The ButtonWithForeground class has an icon and a text.

    \b{This component is under heavy development.}

    A ButtonWithForeground can have text, an icon, or both.
*/
AbstractButton {
    id: button

    /*!
      \preliminary
      The dimensions of the button.
    */
    width: 150
    height: 50

    /*!
       \preliminary
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
       \qmlproperty url iconSource
    */
    property alias iconSource: icon.source

    /*!
       \preliminary
       The text to display in the button. If an icon was defined,
       the text will be shown next to the icon, otherwise it will
       be centered. Leave blank for an icon-only button.
       \qmlproperty string text
    */
    property alias text: label.text

    /*!
      \preliminary
      The size of the text that is displayed in the button.
      \qmlproperty string textSize
    */
    property alias textSize: label.fontSize

    /*!
      \preliminary
      The color of the text.
      \qmlproperty color textColor
    */
    property alias textColor: label.color

    /*!
       \preliminary

       The position of the icon relative to the text. Options
       are "left" and "right". The default value is "left".

       If only text or only an icon is defined, this
       property is ignored and the text or icon is
       centered horizontally and vertically in the button.

       Currently this is a string value. We are waiting for
       support for enums:
       https://bugreports.qt-project.org/browse/QTBUG-14861
    */
    property string iconPosition: "left"

    Image {
        id: icon
        fillMode: Image.PreserveAspectFit
        anchors.margins: 10
        anchors.verticalCenter: button.verticalCenter
        sourceSize.width: width
        sourceSize.height: height
        height: {
            if (text===""||iconPosition=="left"||iconPosition=="right") return button.height - 20;
            else return button.height - label.implicitHeight - 30;
        }
        opacity: button.enabled ? 1.0 : 0.5
     }

    TextCustom {
        id: label
        anchors.margins: 10
        anchors.verticalCenter: button.verticalCenter
        anchors.verticalCenterOffset: -1
        fontSize: "large"
        font.italic: true
        opacity: button.enabled ? 1.0 : 0.5-
    }

    Item { //placed in here to keep state property private
        id: positioner

        states: [
            State {
                name: "right"
                AnchorChanges {
                    target: icon;
                    anchors.right: button.right
                }
                AnchorChanges {
                    target: label;
                    anchors.right: icon.left
                }
            },
            State {
                name: "left"
                AnchorChanges {
                    target: icon;
                    anchors.left: button.left
                }
                AnchorChanges {
                    target: label;
                    anchors.left: icon.right
                }
            },
            State {
                name: "center"
                AnchorChanges {
                    target: icon;
                    anchors.horizontalCenter: button.horizontalCenter
                }
                AnchorChanges {
                    target: label;
                    anchors.horizontalCenter: button.horizontalCenter
                }
            }
        ]
        state: (button.iconSource == "" || button.text == "") ? "center" : iconPosition
    }
}
