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

// FIXME(loicm) The component copies code from Button.qml and
//     AbstractButton.qml, though I guess it could be reused quite easily. I'll
//     need to figure that out.

// FIXME(loicm) An option should be added to render the value text outside of
//     the thumb, on top or bottom of the thumb (or left and right once we got
//     support for vertical orientation). Moreover, having the value text inside
//     the thumb makes it complicated to fit numbers with high values (or high
//     number of decimals) or visually awkward since the text is hidden by the
//     mouse cursor (or the finger) when dragged.

// FIXME(loicm) Maybe add support for vertical orientation (orientation
//     property). Don't forget to adapt the inverted property documentation
//     accordingly. Note that UIKit doesn't have vertical sliders.

// FIXME(loicm) Add support for keyboard shortcuts (basically left/right).

import QtQuick 1.1
import Qt.labs.shaders 1.0

/*!
    \qmlclass Slider
    \inqmlmodule Ubuntu.Components 0.1
    \brief Slider is a component to select a value from a continuous range of
     values.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        Slider {
            function formatValue(v) { return v.toFixed(2) }
            minimumValue: -3.14
            maximumValue: 3.14
            value: 0.0
            live: true
        }
    }
    \endqml
*/
Item {
    id: slider

    // FIXME(loicm) There are stretched pixels on the left of the thumb when
    //     the width is an odd number.
    width: 300

    // FIXME(loicm) Button and other components have a default height of 39, I
    //     had to specify 42 here because under that value the BorderImage of
    //     the inner element gets cropped and starts to look ugly. A general
    //     solution would need to be found in order to support arbitrary sizes,
    //     or maybe simply clamping the requested sizes.
    height: 42

    // FIXME(loicm) Add Support for the inverted property. There's an ongoing
    //     debate on whether we should use that property (like every other
    //     toolkits) or add new enumerations to the orientation property and
    //     get something like LeftToRight, RightToLeft, TopToBottom and
    //     BottomToTop. The last proposition appears to have the advantage that
    //     it's not culture centric (ie inverted for LeftToRight and RightToLeft
    //     isn't natural for everybody).

    // /*!
    //    \preliminary
    //    Selects whether or not the minimum value starts from the right.
    // */
    // property bool inverted: false

    /*!
       \preliminary
       The minimum value from the continuous range of values. If this value is
       greater than maximumValue, the component will be in an inconsistent
       state.
    */
    property real minimumValue: 0.0

    /*!
       \preliminary
       The maximum value from the continuous range of values. If this value is
       lesser than minimumValue, the component will be in an inconsistent state.
    */
    property real maximumValue: 100.0

    // FIXME(loicm) Add Support for the stepSize property.

    // /*!
    //    \preliminary
    //    The distance between two selectable values in the range defined by
    //    [minimumValue, maximumValue].
    // */
    // property real stepSize: 1.0

    /*!
       \preliminary
       The current value of the slider. This property is not changed while the
       thumb is dragged unless the live property is set to true.
    */
    property real value: 0.0

    /*!
       \preliminary
       Defines whether the value is updated while the thumb is dragged or just
       when the thumb is released.
    */
    property bool live: false

    /*!
       \preliminary
       This property is true while the thumb is dragged. This property is meant
       to be read-only.
    */
    property bool pressed: false

    /*!
      \preliminary
      The mouse area of the check box. May be assigned a different mouse area
      if, for example, the area where the control is shown is not the same as
      the area where it accepts mouse events. This is used in list items with
      controls.
     */
    property MouseArea mouseArea: defaultMouseArea

    /*!
      \preliminary
      This function is used by the value indicator to show the current value.
      Reimplement this function if you want to show different information. By
      default, the value v is rounded to the nearest interger value.
     */
    function formatValue(v) {
        return v.toFixed(0)
    }

    MouseArea {
        id: defaultMouseArea
        anchors.fill: parent
    }

    onMouseAreaChanged: p.updateMouseArea()
    onValueChanged: p.value = slider.value

    Item {
        id: main
        anchors.fill: parent

        BorderImage {
            id: backgroundShape
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: p.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: backgroundBase
            anchors.fill: parent
            color: p.backgroundColor
        }

        ButtonMaskEffect {
            anchors.fill: parent
            gradientStrength: 0.0
            mask: ShaderEffectSource { sourceItem: backgroundShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: backgroundBase; live: true; hideSource: true }
        }

        BorderImage {
            id: backgroundBorder
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: p.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        // FIXME(loicm) The radius of the thumb BorderImage must be lower than
        //     the radius of the background BorderImage so that the perimeter can
        //     look perfectly consistent.

        BorderImage {
            id: thumbShape
            x: backgroundShape.x + p.thumbSpacing + p.normalizedValue * p.thumbSpace
            y: backgroundShape.y + p.thumbSpacing
            width: p.thumbWidth
            height: backgroundShape.height - (2.0 * p.thumbSpacing)
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: p.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: thumbBase
            anchors.fill: thumbShape
            color: p.thumbColor
        }

        ButtonMaskEffect {
            anchors.fill: thumbShape
            gradientStrength: slider.pressed ? 0.2 : 0.4
            mask: ShaderEffectSource { sourceItem: thumbShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: thumbBase; live: true; hideSource: true }

            Behavior on gradientStrength {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        BorderImage {
            id: thumbBorder
            anchors.fill: thumbShape
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: p.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
            visible: true
        }

        TextCustom {
            id: thumbValue
            anchors.centerIn: thumbShape
            fontSize: "medium"
            font.weight: Font.Bold
            color: "white"
            text: slider.formatValue(p.clamp(p.value, slider.minimumValue, slider.maximumValue))
        }
    }

    QtObject {
        id: p

        property url shapeSource: Qt.resolvedUrl("artwork/ButtonShape.png")
        property url borderIdleSource: Qt.resolvedUrl("artwork/ButtonBorderIdle.png")
        property real thumbSpacing: 3.0
        property real thumbWidth: 50.0
        property real thumbSpace: backgroundShape.width - (2.0 * p.thumbSpacing + p.thumbWidth)
        property color backgroundColor: "#c4c4c4"
        property color thumbColor: "#8b8b8b"
        property real dragInitMouseX: 0.0
        property real dragInitNormalizedValue: 0.0
        property real value: 0.0
        property real normalizedValue: p.clamp((p.value - slider.minimumValue) /
                                               (slider.maximumValue - slider.minimumValue),
                                               0.0, 1.0)

        // FIXME(loicm) It would be useful to have these functions available
        //     in a global set of common native (C++) functions. It's very
        //     likely that we'll have more candidates along our dev process.

        function clamp(x, min, max) {
            return Math.max(min, Math.min(x, max));
        }

        function lerp(x, a, b) {
            return ((1.0 - x) * a) + (x * b);
        }

        function updateMouseArea() {
            if (slider.mouseArea) {
                slider.mouseArea.pressedChanged.connect(p.mouseAreaPressed);
                slider.mouseArea.positionChanged.connect(p.mouseAreaPositionchanged);
            }
        }

        function mouseAreaPressed() {
            if (slider.mouseArea.pressedButtons == Qt.LeftButton) {
                // Left button pressed.
                var mouseX = slider.mouseArea.mouseX;
                var mouseY = slider.mouseArea.mouseY;
                if (mouseY >= thumbShape.y && mouseY <= thumbShape.y + thumbShape.height) {
                    if (mouseX >= thumbShape.x && mouseX <= thumbShape.x + thumbShape.width) {
                        // Button pressed inside the thumb.
                        p.dragInitMouseX = mouseX;
                        p.dragInitNormalizedValue = p.normalizedValue;
                        slider.pressed = true;
                    } else if (mouseX > p.thumbSpacing &&
                               mouseX < backgroundShape.width - p.thumbSpacing) {
                        // Button pressed outside the thumb.
                        var normalizedPosition = (slider.mouseArea.mouseX - p.thumbSpacing -
                                                  p.thumbWidth * 0.5) / p.thumbSpace;
                        normalizedPosition = p.clamp(normalizedPosition, 0.0, 1.0);
                        p.value = p.lerp(normalizedPosition, slider.minimumValue,
                                         slider.maximumValue);
                        p.dragInitMouseX = mouseX;
                        p.dragInitNormalizedValue = p.normalizedValue;
                        slider.pressed = true;
                        if (slider.live) {
                            slider.value = p.value
                        }
                    }
                }
            } else if (slider.pressed) {
                // Left button released.
                slider.pressed = false;
                if (!slider.live) {
                    slider.value = p.value;
                }
            }
        }

        function mouseAreaPositionchanged() {
            // Left button dragging.
            if (slider.pressed) {
                var normalizedOffsetX = (slider.mouseArea.mouseX - p.dragInitMouseX) / p.thumbSpace;
                var v = p.clamp(p.dragInitNormalizedValue + normalizedOffsetX, 0.0, 1.0);
                p.value = p.lerp(v, slider.minimumValue, slider.maximumValue);
                if (slider.live) {
                    slider.value = p.value
                }
            }
        }
    }
}
