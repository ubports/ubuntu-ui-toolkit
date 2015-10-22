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
import "../ListItems/1.3"

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
        left: parent.left
        right: parent.right
        bottom: parent.bottom
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
      \qmlproperty string text
      The label displayed by the BottomEdgeHint.
     */
    property alias text: label.text

    /*!
      \qmlproperty url iconSource
      The icon displayed by the BottomEdgeHint.

      This is the URL of any image file.
      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property alias iconSource: icon.source

    /*!
      \qmlproperty string iconName
      The icon associated with the BottomEdgeHint in the icon theme.

      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property alias iconName: icon.name

    /*!
      \qmlproperty string state
      BottomEdgeHint can take 2 states of visibility: "Hidden" and "Visible".

      When "Visible", the full hint with its content is shown.

      When "Hidden", only part of the hint is visible leaving more space for application content.
     */
    state: "Hidden"

    states: [
        State {
            name: "Visible"
            PropertyChanges {
                target: h1
                anchors.verticalCenterOffset: bottomEdgeHint.height / 2
            }
            PropertyChanges {
                target: h2
                anchors.topMargin: 0
            }
        }
    ]
    transitions: Transition {
        from: "Hidden"
        to: "Visible"
        reversible: true
        UbuntuNumberAnimation {
            targets: [h1, h2]
            properties: "anchors.verticalCenterOffset, anchors.topMargin"
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        onEntered: bottomEdgeHint.state = "Visible"
        onExited: hidingTimer.start()
    }

    Timer {
        id: hidingTimer
        interval: 800
        repeat: false
        onTriggered: bottomEdgeHint.state = "Hidden"
    }

    clip: true

    Icon {
        id: h1
        width: units.gu(2)
        height: width
        anchors {
            centerIn: parent
            topMargin: bottomEdgeHint.height
        }
        name: "up"
    }

    Rectangle {
        id: h2
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: bottomEdgeHint.height
        }
        height: bottomEdgeHint.height
        color: theme.palette.normal.overlay
        ThinDivider {
            anchors.top: parent.top
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Haptics.play();
                bottomEdgeHint.clicked();
                mouse.accepted = false;
            }
        }

        Row {
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            spacing: units.gu(1)
            Icon {
                id: icon
                width: height
                height: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                color: theme.palette.normal.overlayText
            }
            Label {
                id: label
                textSize: Label.Medium
                color: theme.palette.normal.overlayText
                height: bottomEdgeHint.height
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
