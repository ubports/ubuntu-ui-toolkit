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

/*!
    The component defines the style of the ComboButton component.
  */
Item {
    id: comboStyle

    /*!
      Width of the drop down button.
      */
    property real dropDownWidth: units.gu(5)

    /*!
      Width of the dropdown button separator.
      */
    property real dropDownSeparatorWidth: units.dp(2)

    /*!
      Distance between the combo list and the main button.
      */
    property real comboListMargin: units.gu(0.8)

    /*!
      The item which will hold the combo list data.
      */
    property Item comboList: comboListContent

    /*!
      The item pointing to the panel holding the combo list and additional design
      artifacts. It is used by the component to drive the expansion size.
      */
    property Item comboListPanel: panelItem

    /*!
      \qmlproperty color defaultColor
      Default color for the main button.
      */
    property alias defaultColor: mainButton.defaultColor

    /*!
      \qmlproperty Gradient defaultGradient
      Default gradient for the main button.
      */
    property alias defaultGradient: mainButton.defaultGradient

    /*!
      Default color for the dropdown button when released.
      */
    property color dropDownColorReleased: defaultColor
    /*!
      Default color for dropdown button when pressed.
      */
    property color dropDownColorPressed: Qt.rgba(0, 0, 0, 0.05)

    width: combo.width
    height: combo.collapsedHeight

    /*! \internal */
    property ComboButton combo: styledItem

    implicitWidth: units.gu(36)
    implicitHeight: units.gu(4)

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    // Color properties in a JS ternary operator don't work as expected in
    // QML because it overwrites alpha values with 1. A workaround is to use
    // Qt.rgba(). For more information, see
    // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1197802 and
    // https://bugreports.qt-project.org/browse/QTBUG-32238.
    /*! \internal */
    function colorHack(color) { return Qt.rgba(color.r, color.g, color.b, color.a); }

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
                color: combo.expanded ? comboStyle.colorHack(comboStyle.dropDownColorPressed) : comboStyle.dropDownColorReleased
                Image {
                    source: "artwork/chevron.png"
                    anchors.centerIn: parent
                    rotation: combo.expanded ? 90 : -90
                }
            }
        }
    }

    Item {
        id: panelItem
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
            color: comboStyle.dropDownColorPressed
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
            source: "artwork/bubble_shadow.sci"
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
