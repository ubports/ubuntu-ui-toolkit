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
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3 as Style

Style.ComboButtonStyle {
    id: comboStyle

    // configurations
    dropDownWidth: units.gu(5)
    comboListMargin: units.gu(0.8)
    comboListHolder: comboListContent
    comboListPanel: panelItem
    defaultColor: mainButton.defaultColor
    defaultGradient: mainButton.defaultGradient
    defaultDropdownColor: mainButton.defaultColor
    defaultFont: mainButton.defaultFont

    width: styledItem.width
    height: styledItem.collapsedHeight

    implicitWidth: mainButton.implicitWidth
    implicitHeight: mainButton.implicitHeight

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    Item {
        id: mainButton

        property real minimumWidth: units.gu(36)
        property real horizontalPadding: units.gu(4)
        property color defaultColor: theme.palette.normal.foreground
        property font defaultFont: Qt.font({family: "Ubuntu", pixelSize: FontUtils.sizeToPixels("medium")})
        property Gradient defaultGradient: null
        property real buttonFaceOffset: -dropDownWidth/2
        property bool stroke: false

        /*!
          The property overrides the button's default background with an item. This
          item can be used by derived styles to reuse the ButtonStyle and override
          the default coloured background with an image or any other drawing.
          The default value is null.
        */
        property Item backgroundSource: comboFace

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        width: styledItem.width
        height: styledItem.collapsedHeight
        implicitWidth: Math.max(
            minimumWidth, foreground.implicitWidth + 2 * mainButton.horizontalPadding
        )
        implicitHeight: units.gu(4)

        LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
        LayoutMirroring.childrenInherit: true

        UbuntuShape {
            id: background
            anchors.fill: parent
            borderSource: "radius_idle.sci"  // Deprecated, use a dedicated shape.
            visible: mainButton.stroke ? false : ((backgroundColor.a != 0.0) || mainButton.backgroundSource)
            source: mainButton.backgroundSource

            backgroundColor: defaultColor
            backgroundMode: UbuntuShape.SolidColor
            opacity: styledItem.enabled ? 1.0 : 0.6
        }

        Item {
            id: foreground

            property string iconPosition: styledItem.iconPosition
            property real iconSize: units.gu(2)
            property real spacing: mainButton.horizontalPadding
            property bool hasIcon: styledItem.iconSource != ""
            property bool hasText: styledItem.text != ""

            anchors {
                centerIn: parent
                horizontalCenterOffset: mainButton.buttonFaceOffset
            }
            transformOrigin: Item.Top
            width: parent.width - 2*mainButton.horizontalPadding
            implicitHeight: Math.max(foregroundIcon.height, foregroundLabel.height)
            scale: styledItem.pressed ? 0.98 : 1.0
            Behavior on scale {
                NumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing.type: Easing.Linear
                }
            }
            state: foreground.hasIcon && hasText ? iconPosition : "center"

            Icon {
                id: foregroundIcon
                visible: foreground.hasIcon
                anchors.verticalCenter: parent.verticalCenter
                width: foreground.iconSize
                height: foreground.iconSize
                color: foregroundLabel.color
                source: styledItem.iconSource
            }

            Label {
                id: foregroundLabel
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
                color: theme.palette.normal.foregroundText
                font: styledItem.font
                text: styledItem.text
            }

            states: [
                State {
                    name: "left"
                    AnchorChanges {
                        target: foregroundIcon
                        anchors.left: foreground.left
                    }
                    AnchorChanges {
                        target: foregroundLabel
                        anchors.left: foregroundIcon.right
                    }
                    PropertyChanges {
                        target: foregroundLabel
                        anchors.leftMargin: spacing
                        width: foreground.width - foregroundIcon.width - spacing
                    }
                    PropertyChanges {
                        target: foreground
                        implicitWidth: foregroundIcon.implicitWidth + spacing + foregroundLabel.implicitWidth
                    }
                },
                State {
                    name: "right"
                    AnchorChanges {
                        target: foregroundIcon
                        anchors.right: foreground.right
                    }
                    AnchorChanges {
                        target: foregroundLabel
                        anchors.right: foregroundIcon.left
                    }
                    PropertyChanges {
                        target: foregroundLabel
                        anchors.rightMargin: spacing
                        width: foreground.width - foregroundIcon.width - spacing
                    }
                    PropertyChanges {
                        target: foreground
                        implicitWidth: foregroundIcon.implicitWidth + spacing + foregroundLabel.implicitWidth
                    }
                },
                State {
                    name: "center"
                    AnchorChanges {
                        target: foregroundIcon
                        anchors.horizontalCenter: foreground.horizontalCenter
                    }
                    AnchorChanges {
                        target: foregroundLabel
                        anchors.horizontalCenter: foreground.horizontalCenter
                    }
                    PropertyChanges {
                        target: foregroundLabel
                        width: Math.min(foregroundLabel.implicitWidth, foreground.width)
                    }
                    PropertyChanges {
                        target: foreground
                        implicitWidth: foreground.hasText ? foregroundLabel.implicitWidth : foregroundIcon.implicitWidth
                    }
                }
            ]
        }

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

            // button background
            Rectangle {
                anchors {
                    fill: parent
                    rightMargin: comboStyle.dropDownWidth
                }
                color: defaultColor
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
                color: defaultDropdownColor
                Icon {
                    anchors.centerIn: parent
                    height: parent.height / 2
                    width: height
                    name: styledItem.expanded? "up" : "down"
                    color: foregroundLabel.color
                }
            }
        }

        FocusShape {
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
        // Hide panel to suppress keyboard input including (Shift)Tab
        visible: styledItem.expanded && (styledItem.comboList.length > 0)

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
            color: defaultDropdownColor
        }

        UbuntuShape {
            id: shape
            anchors {
                fill: parent
                topMargin: comboListMargin
            }
            source: listContent
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
