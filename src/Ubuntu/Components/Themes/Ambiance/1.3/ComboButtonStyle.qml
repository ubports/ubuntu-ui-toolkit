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

    property ComboButton combo: styledItem

    // configurations
    dropDownWidth: units.gu(5)
    dropDownSeparatorWidth: units.dp(2)
    comboListMargin: units.gu(0.8)
    comboListHolder: comboListContent
    comboListPanel: panelItem
    defaultColor: mainButton.defaultColor
    defaultGradient: mainButton.defaultGradient
    defaultDropdownColor: combo.expanded ? Qt.rgba(0, 0, 0, 0.05) : mainButton.defaultColor
    defaultFont: mainButton.defaultFont

    width: combo.width
    height: combo.collapsedHeight

    implicitWidth: mainButton.implicitWidth
    implicitHeight: mainButton.implicitHeight

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    Item {
        id: mainButton

        property real minimumWidth: units.gu(36)
        property real horizontalPadding: units.gu(4) - dropDownSeparatorWidth
        // FIXME: Add this color to the palette
        property color defaultColor: "#b2b2b2"
        property font defaultFont: Qt.font({family: "Ubuntu", pixelSize: FontUtils.sizeToPixels("medium")})
        property Gradient defaultGradient: null
        property real buttonFaceOffset: -dropDownWidth/2 - dropDownSeparatorWidth
        property bool stroke: styledItem.hasOwnProperty("strokeColor") && styledItem.strokeColor != Qt.rgba(0.0, 0.0, 0.0, 0.0)
        /*!
          The property overrides the button's default background with an item. This
          item can be used by derived styles to reuse the ButtonStyle and override
          the default coloured background with an image or any other drawing.
          The default value is null.
          */
        property Item backgroundSource: comboFace

        /* The proxy is necessary because Gradient.stops and GradientStop.color are
           non-NOTIFYable properties. They cannot be written to so it is fine but
           the proxy avoids the warnings.
        */
        property QtObject gradientProxy: gradientProxyObject
        QtObject {
            id: gradientProxyObject
            property color topColor
            property color bottomColor

            function updateGradient() {
                if (styledItem.gradient) {
                    topColor = styledItem.gradient.stops[0].color;
                    bottomColor = styledItem.gradient.stops[1].color;
                }
            }

            Component.onCompleted: {
                updateGradient();
                styledItem.gradientChanged.connect(updateGradient);
            }
        }

        // Use the gradient if it is defined and the color has not been set
        // manually or the gradient has been set manually
        property bool isGradient: styledItem.gradient && (
            styledItem.color == defaultColor || styledItem.gradient != defaultGradient
        )

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        width: styledItem.width
        height: combo.collapsedHeight
        implicitWidth: Math.max(
            minimumWidth, foreground.implicitWidth + 2 * mainButton.horizontalPadding
        )
        implicitHeight: units.gu(4)

        LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
        LayoutMirroring.childrenInherit: true

        Image {
            id: strokeBorder
            anchors.fill: parent
            anchors.margins: -units.gu(0.5)
            // FIXME: this PNG is way too big (462x108) and do not scale properly
            // ie. the corners are visually incorrect at most sizes
            source: mainButton.stroke ? Qt.resolvedUrl("../artwork/stroke_button.png") : ""
            visible: false
            cache: false
            asynchronous: true
        }

        ShaderEffect {
            id: colorizedImage

            anchors.fill: parent
            visible: mainButton.stroke && strokeBorder.status == Image.Ready

            property Item source: visible ? strokeBorder : null
            property color keyColorOut: mainButton.stroke ? styledItem.strokeColor : Qt.rgba(0.0, 0.0, 0.0, 0.0)
            property color keyColorIn: Qt.rgba(1.0, 1.0, 1.0, 1.0)
            property real threshold: 1.0

            fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform highp vec4 keyColorOut;
            uniform highp vec4 keyColorIn;
            uniform lowp float threshold;
            uniform lowp float qt_Opacity;
            void main() {
                lowp vec4 sourceColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(keyColorOut.rgb, 1.0) * sourceColor.a, sourceColor, step(threshold, distance(sourceColor.rgb / sourceColor.a, keyColorIn.rgb))) * qt_Opacity;
            }"
        }

        UbuntuShape {
            id: background
            anchors.fill: parent
            borderSource: "radius_idle.sci"  // Deprecated, use a dedicated shape.
            visible: mainButton.stroke ? false : ((backgroundColor.a != 0.0) || mainButton.backgroundSource)
            source: mainButton.backgroundSource

            backgroundColor: mainButton.backgroundSource ? "#00000000" : (mainButton.isGradient ? gradientProxy.topColor : styledItem.color)
            secondaryBackgroundColor: mainButton.backgroundSource ? "#00000000" : (mainButton.isGradient ? gradientProxy.bottomColor : styledItem.color)
            backgroundMode: mainButton.isGradient ? UbuntuShape.VerticalGradient : UbuntuShape.SolidColor
            opacity: styledItem.enabled ? 1.0 : 0.6
        }

        UbuntuShape {
            id: backgroundPressed
            anchors.fill: parent
            backgroundColor: mainButton.stroke ? styledItem.strokeColor : background.backgroundColor
            secondaryBackgroundColor: background.secondaryBackgroundColor
            backgroundMode: mainButton.stroke ? UbuntuShape.SolidColor : UbuntuShape.VerticalGradient
            borderSource: "radius_pressed.sci"  // Deprecated, use a dedicated shape.
            opacity: styledItem.pressed ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing.type: Easing.Linear
                }
            }
            visible: mainButton.stroke || background.visible
        }

        Item {
            id: foreground

            property string iconPosition: styledItem.iconPosition
            property real iconSize: units.gu(3)
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
                source: combo.iconSource
            }

            Label {
                id: foregroundLabel
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
                /* Pick either a clear or dark text color depending on the
                 * luminance of the background color to maintain good contrast
                 * (works in most cases)
                 */
                color: ColorUtils.luminance(combo.color) <= 0.85 && !(mainButton.stroke && !combo.pressed) ? "#FFFFFF" : "#888888"
                font: styledItem.font
                text: combo.text
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
                color: combo.dropdownColor
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
            color: combo.dropdownColor
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
            source: listContent
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
