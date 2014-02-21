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

    property real dropDownSeparatorWidth: units.gu(0.5)
    property real dropDownWidth: units.gu(4)
    property Item comboList: comboListHolder

//    implicitHeight: units.gu(5)
//    implicitWidth: units.gu(14)
    width: combo.width
    height: combo.collapsedHeight

    property ComboButton combo: styledItem
    property real minimumWidth: units.gu(15)
    property real horizontalPadding: units.gu(1)
    property real buttonFaceOffset: -dropDownWidth / 2
    property color defaultColor
    property Gradient defaultGradient: UbuntuColors.orangeGradient

    implicitWidth: Math.max(minimumWidth, foreground.implicitWidth + 2*horizontalPadding)
    implicitHeight: units.gu(5)

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

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
            if (combo.gradient) {
                topColor = combo.gradient.stops[0].color;
                bottomColor = combo.gradient.stops[1].color;
            }
        }

        Component.onCompleted: {
            updateGradient();
            combo.gradientChanged.connect(updateGradient);
            print(combo, combo.gradient, combo.color)
        }
        function colorHack(color) { return Qt.rgba(color.r, color.g, color.b, color.a); }
    }

    // Use the gradient if it is defined and the color has not been set manually
    // or the gradient has been set manually
    property bool isGradient: combo.gradient && (combo.color == defaultColor ||
                              combo.gradient != defaultGradient)

    Item {
        id: mainButton
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        height: combo.collapsedHeight

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
//                gradient: {print("G", combo, combo.gradient); combo.gradient}
//                color: {print("C", combo, combo.color); combo.color}
                color: isGradient ? "#00000000" : gradientProxy.colorHack(combo.color)
                gradient: isGradient ? combo.gradient : null
            }

            Rectangle {
                anchors {
                    right: dropDownButton.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: comboStyle.dropDownSeparatorWidth
                color: Theme.palette.normal.background
            }

            Rectangle {
                id: dropDownButton
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: comboStyle.dropDownWidth
                color: Theme.palette.normal.overlay
            }
        }
        UbuntuShape {
            id: background
            anchors.fill: parent
            borderSource: "radius_idle.sci"
            visible: true
            image: comboFace

//            // Color properties in a JS ternary operator don't work as expected in
//            // QML because it overwrites alpha values with 1. A workaround is to use
//            // Qt.rgba(). For more information, see
//            // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1197802 and
//            // https://bugreports.qt-project.org/browse/QTBUG-32238.
//            function colorHack(color) { return Qt.rgba(color.r, color.g, color.b, color.a); }
//            color: "#00000000"
//            gradientColor: "#00000000"
        }

        UbuntuShape {
            id: backgroundPressed
            anchors.fill: parent
            color: background.color
            gradientColor: background.gradientColor
            borderSource: "radius_pressed.sci"
            opacity: combo.pressed ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing.type: Easing.Linear
                }
            }
            visible: background.visible
        }

        ButtonForeground {
            id: foreground
            width: parent.width - 2*horizontalPadding
            anchors {
                centerIn: parent
                horizontalCenterOffset: buttonFaceOffset
            }
            text: combo.text
            /* Pick either a clear or dark text color depending on the luminance of the
               background color to maintain good contrast (works in most cases)
            */
            textColor: ColorUtils.luminance(combo.color) <= 0.85 ? "#F3F3E7" : "#888888"
            iconSource: combo.iconSource
            iconPosition: combo.iconPosition
            iconSize: units.gu(3)
            spacing: horizontalPadding
            transformOrigin: Item.Top
            scale: combo.pressed ? 0.98 : 1.0
            Behavior on scale {
                NumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing.type: Easing.Linear
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
        clip: true

        Behavior on height {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
    }
}




//Item {
//    id: comboStyle
//    property real dropDownSeparatorWidth: units.gu(0.5)
//    property real dropDownWidth: units.gu(4)
//    property Item comboList: comboListHolder

//    property alias defaultColor: mainButton.defaultColor
//    property alias defaultGradient: mainButton.defaultGradient
//    implicitHeight: units.gu(5)
//    implicitWidth: units.gu(14)

//    property ComboButton combo: styledItem
//    Component.onCompleted: print(combo.gradient, combo.color)

//    comboStyle {
//        id: mainButton
//        anchors {
//            left: parent.left
//            right: parent.right
//            top: parent.top
//        }
//        height: units.gu(5)

//        // overrides of the common style
////        defaultColor: comboStyle.defaultColor
////        defaultGradient: comboStyle.defaultGradient
//        minimumWidth: units.gu(14)
//        buttonFaceOffset: -dropDownWidth / 2
//        sourceItem: comboFace
//        ShaderEffectSource {
//            id: comboFace
//            sourceItem: content
//            hideSource: true
//            // FIXME: visible: false prevents rendering so make it a nearly
//            // transparent 1x1 pixel instead
//            opacity: 0.01
//            width: 1
//            height: 1
//        }

//        Item {
//            id: content
//            width: mainButton.width
//            height: mainButton.height

//            Rectangle {
//                anchors {
//                    fill: parent
//                    rightMargin: comboStyle.dropDownSeparatorWidth + comboStyle.dropDownWidth
//                }
//                gradient: {print("G", combo, combo.gradient); combo.gradient}
//                color: {print("C", combo, combo.color); combo.color}
//            }

//            Rectangle {
//                anchors {
//                    right: dropDownButton.right
//                    top: parent.top
//                    bottom: parent.bottom
//                }
//                width: comboStyle.dropDownSeparatorWidth
//                color: Theme.palette.normal.background
//            }

//            Rectangle {
//                id: dropDownButton
//                anchors {
//                    right: parent.right
//                    top: parent.top
//                    bottom: parent.bottom
//                }
//                width: comboStyle.dropDownWidth
//                color: Theme.palette.normal.overlay
//            }
//        }
//    }

//    Connections {
//        target: combo
//        onExpandedChanged: {
//            if (expanded)
//                comboListHolder.show()
//            else
//                comboListHolder.hide()
//        }
//    }

//    Popover {
//        id: comboListHolder
//        anchors {
//            left: parent.left
//            top: mainButton.bottom
//            right: parent.right
//        }
//        height: combo.expanded ? (combo.expandedHeight - combo.collapsedHeight) : 0
//        clip: true
//        caller: dropDownButton

//        Behavior on height {
//            NumberAnimation {
//                duration: UbuntuAnimation.SnapDuration
//                easing.type: Easing.Linear
//            }
//        }
//    }
//}
