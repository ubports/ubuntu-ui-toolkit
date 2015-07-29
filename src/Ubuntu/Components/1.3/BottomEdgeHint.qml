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
import Ubuntu.Components 1.3

/*!
    \qmlabstract BottomEdgeHint
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief The BottomEdgeHint shows the availability of extra features
    available from the bottom edge of the application.

    It displays either a label or an icon at the bottom of the application.

    It has 2 states: hidden or visible. When hidden, part of it is still visible
    to hint at the existence of the bottom edge.

    When used with a mouse it acts like a button. The typical action associated
    with clicking on it should be revealing the extra features provided by the
    bottom edge.

    Example:
    \qml
    BottomEdgeHint {
        id: bottomEdgeHint
        text: i18n.tr("Favorites")
        onClicked: revealBottomEdge()
    }
    \endqml

*/
Item {
    id: bottomEdgeHint

    anchors {
        bottom: parent.bottom
        bottomMargin: bottomEdgeHint.state == "Hidden" ? -bottomEdgeHint.height + units.gu(1.5) : 0
        horizontalCenter: parent.horizontalCenter
        Behavior on bottomMargin { UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration } }
    }

    width: label.paintedWidth + units.gu(7)
    height: units.gu(4)

    /*!
       This handler is called when there is a mouse click on the BottomEdgeHint
       and the BottomEdgeHint is not disabled.
    */
    signal clicked()

    Keys.onEnterPressed: clicked()
    Keys.onReturnPressed: clicked()

    /*!
      The label displayed by the BottomEdgeHint.
     */
    property string text

    /*!
      The icon displayed by the BottomEdgeHint.

      This is the URL of any image file.
      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property url iconSource: iconName ? "image://theme/" + iconName : ""

    /*!
      The icon associated with the BottomEdgeHint in the icon theme.

      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property string iconName

    /*!
      BottomEdgeHint can take 2 states of visibility: "Hidden" and "Visible".

      When "Visible", the full hint with its content is shown.

      When "Hidden", only part of the hint is visible leaving more space for application content.
     */
    property string state: "Visible"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: bottomEdgeHint.state = "Visible"
        onClicked: {
            Haptics.play();
            bottomEdgeHint.clicked();
            mouse.accepted = false;
        }
    }

    clip: true

    UbuntuShape {
        id: background

        anchors {
            bottom: parent.bottom
            horizontalCenter: bottomEdgeHint.horizontalCenter
            bottomMargin: -units.gu(1)
        }

        width: bottomEdgeHint.width - 2 * hoverExpansion
        height: bottomEdgeHint.height + units.gu(1) - hoverExpansion

        property real hoverExpansion: mouseArea.containsMouse ? 0 : units.gu(0.5)
        Behavior on hoverExpansion { UbuntuNumberAnimation { duration: UbuntuAnimation.FastDuration } }

        backgroundColor: theme.palette.normal.overlay
    }

    Label {
        id: label

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        text: icon.name ? "" : bottomEdgeHint.text
        fontSize: "medium"
        height: bottomEdgeHint.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Icon {
        id: icon

        name: bottomEdgeHint.iconName
        anchors {
            bottom: parent.bottom
            bottomMargin: units.gu(0.5)
            horizontalCenter: parent.horizontalCenter
        }
        width: height
        height: units.gu(2)
    }
}
