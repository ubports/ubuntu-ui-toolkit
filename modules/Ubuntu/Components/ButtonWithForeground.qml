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

/*!
    \qmlclass ButtonWithForeground
    \inqmlmodule Ubuntu.Components 0.1
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
    width: units.dp(150)
    height: units.dp(50)

    /*!
       \preliminary
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
       \qmlproperty url iconSource
    */
    property alias iconSource: icon.resource

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

    implicitWidth: foreground.implicitWidth + 2*foreground.horizontalPadding

    Item {
        id: foreground
        anchors.centerIn: button

        /*!
          \preliminary
          The padding on the left and right side of the button foreground.
        */
        property real horizontalPadding: units.dp(10)
        property real maximumWidth: button.width - 2*foreground.horizontalPadding
        property real spacing: (button.text == "" || button.iconSource == "") ? 0 : units.dp(10)
        property real verticalPadding: units.dp(5)

        width: icon.width + label.width + foreground.spacing
        height: button.height - 2*foreground.verticalPadding
        implicitWidth: icon.width + label.implicitWidth + foreground.spacing

        ImageMipmap {
            id: icon
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: foreground.verticalCenter
            opacity: button.enabled ? 1.0 : 0.5
        }

        TextCustom {
            id: label
            anchors {
                verticalCenter: foreground.verticalCenter
                verticalCenterOffset: units.dp(-1)
            }
            fontSize: "medium"
            font.italic: true
            opacity: button.enabled ? 1.0 : 0.5
            elide: Text.ElideRight

            property real availableWidth: foreground.maximumWidth - icon.width - foreground.spacing
            width: (implicitWidth < availableWidth) ? implicitWidth : availableWidth
        }
    }

    Item { //placed in here to keep state property private
        id: positioner

        states: [
            State {
                name: "right"
                AnchorChanges {
                    target: icon;
                    anchors.right: foreground.right
                }
                AnchorChanges {
                    target: label;
                    anchors.left: foreground.left
                }
            },
            State {
                name: "left"
                AnchorChanges {
                    target: icon;
                    anchors.left: foreground.left
                }
                AnchorChanges {
                    target: label;
                    anchors.right: foreground.right
                }
            },
            State {
                name: "center"
                AnchorChanges {
                    target: icon;
                    anchors.horizontalCenter: foreground.horizontalCenter
                }
                AnchorChanges {
                    target: label;
                    anchors.horizontalCenter: foreground.horizontalCenter
                }
            }
        ]
        state: (button.iconSource == "" || button.text == "") ? "center" : iconPosition
    }
}
