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

import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.Styles 1.2 as Style

Style.ComboButtonStyle {
    id: comboStyle

    // configurations
    dropDownWidth: units.gu(5)
    dropDownSeparatorWidth: units.dp(2)
    comboListMargin: units.gu(0.8)
    comboListHolder: comboListContent
    comboListPanel: panelItem
    defaultColor: mainButton.defaultColor
    defaultGradient: mainButton.defaultGradient
    defaultDropdownColor: combo.expanded ? Qt.rgba(0, 0, 0, 0.05) : defaultColor
    defaultFont: mainButton.defaultFont


    width: combo.width
    height: combo.collapsedHeight

    property ComboButton combo: styledItem

    implicitWidth: mainButton.implicitWidth
    implicitHeight: mainButton.implicitHeight

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

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
        buttonFaceOffset: -dropDownWidth/2 - dropDownSeparatorWidth
        horizontalPadding: units.gu(4) - dropDownSeparatorWidth
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
                objectName: "combobutton_dropdown_visuals"
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: comboStyle.dropDownWidth
                color: mainButton.__colorHack(combo.dropdownColor)
                Image {
                    source: Qt.resolvedUrl("../artwork/chevron.png")
                    anchors.centerIn: parent
                    rotation: combo.expanded ? -90 : 90
                }
            }
        }
    }

    Item {
        id: panelItem
        objectName: "combobutton_combopanel"
        anchors {
            left: parent.left
            top: mainButton.bottom
            right: parent.right
        }
        opacity: combo.expanded && (combo.comboList.length > 0)? 1.0 : 0.0

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
        Rectangle {
            id: comboListContent
            anchors {
                fill: parent
                topMargin: comboListMargin
            }
            clip: true
            color: mainButton.__colorHack(combo.dropdownColor)
        }

        BorderImage {
            id: shadow
            anchors {
                fill: parent
                leftMargin: -units.gu(0.5)
                topMargin: comboListMargin - units.gu(0.5)
                rightMargin: -units.gu(0.5)
                bottomMargin: -units.gu(0.5)
            }
            source: Qt.resolvedUrl("../artwork/bubble_shadow.sci")
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
            source: Qt.resolvedUrl("../artwork/bubble_arrow.png")
            rotation: 180
            anchors {
                bottom: shape.top
                bottomMargin: -1
                right: parent.right
                rightMargin: dropDownWidth / 2 - units.gu(0.5)
            }

        }

        Behavior on height {
            NumberAnimation {
                duration: UbuntuAnimation.FastDuration
                easing: UbuntuAnimation.StandardEasing
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: UbuntuAnimation.FastDuration
                easing: UbuntuAnimation.StandardEasing
            }
        }
    }
}
