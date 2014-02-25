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

Item {
    id: comboStyle

    property real dropDownSeparatorWidth: units.dp(2)
    property real dropDownWidth: units.gu(5)
    property real comboListMargin: units.gu(1)
    property Item comboList: comboListContent

    width: combo.width
    height: combo.collapsedHeight

    property ComboButton combo: styledItem
    property alias defaultColor: mainButton.defaultColor
    property alias defaultGradient: mainButton.defaultGradient

    implicitWidth: units.gu(36)
    implicitHeight: units.gu(4)

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    //Update component's height
    Binding {
        target: combo
        property: "height"
        value: combo.collapsedHeight + comboListHolder.height
    }

    ButtonStyle {
        id: mainButton
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        height: combo.collapsedHeight
        // overrides
        backgroundSource: comboFace
        buttonFaceOffset: -dropDownWidth / 2
        minimumWidth: units.gu(36)

        // FIXME: use hardcoded color while we get the theme palette updated
        defaultColor: "#b2b2b2"
        defaultGradient: null

        // button face
        ShaderEffectSource {
            id: comboFace
            sourceItem: content
            hideSource: true
            // FIXME: visible: false prevents rendering so make it a nearly
            // transparent 1x1 pixel instead
            opacity: 0.01
            width: 1
            height: 1
        }

        Item {
            id: content
            width: mainButton.width
            height: mainButton.height

            Rectangle {
                anchors {
                    fill: parent
                    rightMargin: comboStyle.dropDownSeparatorWidth + comboStyle.dropDownWidth
                }
                color: combo.color
                gradient: mainButton.isGradient ? combo.gradient : null
            }

            // distancer
            Item {
                anchors {
                    right: dropDownButton.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: comboStyle.dropDownSeparatorWidth
            }

            Rectangle {
                id: dropDownButton
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: comboStyle.dropDownWidth
                color: !combo.expanded ? defaultColor : Qt.rgba(0, 0, 0, 0.05)
                Icon {
                    name: combo.expanded ? "go-up" : "go-down"
                    anchors.centerIn: parent
                }
            }
        }
    }

    Item {
        id: comboListHolder
        anchors {
            left: parent.left
            top: mainButton.bottom
            right: parent.right
        }
        height: combo.expanded ? (combo.expandedHeight - combo.collapsedHeight) : 0

        ShaderEffectSource {
            id: listContent
            sourceItem: comboListContent
            hideSource: true
            // FIXME: visible: false prevents rendering so make it a nearly
            // transparent 1x1 pixel instead
            opacity: 0.01
            width: 1
            height: 1
        }
        Item {
            id: comboListContent
            anchors {
                fill: parent
                topMargin: comboListMargin
            }
            clip: true
        }

        UbuntuShape {
            id: shape
            anchors {
                fill: parent
                topMargin: comboListMargin
            }
            visible: true
            borderSource: "radius_idle.sci"
            image: listContent
        }

        Image {
            source: "artwork/bubble_arrow.png"
            rotation: 180
            anchors {
                bottom: shape.top
                right: parent.right
                rightMargin: dropDownWidth / 2
            }

        }

        Behavior on height {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
    }
}
