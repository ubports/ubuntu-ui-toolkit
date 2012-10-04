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

// FIXME(loicm) An option should be added to render the value text outside of
//     the thumb, on top or bottom of the thumb (or left and right once we got
//     support for vertical orientation). Moreover, having the value text inside
//     the thumb makes it complicated to fit numbers with high values (or high
//     number of decimals) or visually awkward since the text is hidden by the
//     mouse cursor (or the finger) when dragged.

// FIXME(loicm) An animation should be added when the user clicks on an area
//     outside the thumb to smoothly move it under the mouse.

// FIXME(loicm) Maybe add support for vertical orientation (orientation
//     property). Don't forget to adapt the inverted property documentation
//     accordingly. Note that UIKit doesn't have vertical sliders.

// FIXME(loicm) Add support for keyboard shortcuts (basically left/right).

import QtQuick 2.0
import "mathUtils.js" as MathUtils

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
AbstractButton {
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

    pressed: false

    /*!
      \preliminary
      This function is used by the value indicator to show the current value.
      Reimplement this function if you want to show different information. By
      default, the value v is rounded to the nearest interger value.
     */
    function formatValue(v) {
        return v.toFixed(0)
    }

    /*! \internal */
    onMouseAreaChanged: __updateMouseArea()

    /*! \internal */
    onValueChanged: __value = slider.value

    Item {
        id: main
        anchors.fill: parent

        BorderImage {
            id: backgroundShape
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: __shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: backgroundBase
            anchors.fill: parent
            color: __backgroundColor
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
            source: __borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        // FIXME(loicm) The radius of the thumb BorderImage must be lower than
        //     the radius of the background BorderImage so that the perimeter can
        //     look perfectly consistent.

        BorderImage {
            id: thumbShape
            x: backgroundShape.x + __thumbSpacing + __normalizedValue * __thumbSpace
            y: backgroundShape.y + __thumbSpacing
            width: __thumbWidth
            height: backgroundShape.height - (2.0 * __thumbSpacing)
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: __shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: thumbBase
            anchors.fill: thumbShape
            color: __thumbColor
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
            source: __borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
            visible: true
        }

        TextCustom {
            id: thumbValue
            anchors {
                verticalCenter: thumbShape.verticalCenter
                left: thumbShape.left
                right: thumbShape.right
            }
            horizontalAlignment: Text.AlignHCenter
            fontSize: "medium"
            font.weight: Font.Bold
            color: "white"
            text: slider.formatValue(MathUtils.clamp(__value, slider.minimumValue,
                                                     slider.maximumValue))
        }
    }

    // Private symbols.

    /*! \internal */
    property url __shapeSource: Qt.resolvedUrl("artwork/ButtonShape.png")
    
    /*! \internal */
    property url __borderIdleSource: Qt.resolvedUrl("artwork/ButtonBorderIdle.png")
    
    /*! \internal */
    property real __thumbSpacing: 3.0
    
    /*! \internal */
    property real __thumbWidth: 50.0
    
    /*! \internal */
    property real __thumbSpace: backgroundShape.width - (2.0 * __thumbSpacing + __thumbWidth)
    
    /*! \internal */
    property color __backgroundColor: "#c4c4c4"
    
    /*! \internal */
    property color __thumbColor: "#8b8b8b"
    
    /*! \internal */
    property real __dragInitMouseX: 0.0

    /*! \internal */
    property real __dragInitNormalizedValue: 0.0

    /*! \internal */
    property real __value: 0.0

    /*! \internal */
    property real __normalizedValue: MathUtils.clamp((__value - slider.minimumValue) /
                                                     (slider.maximumValue - slider.minimumValue),
                                                     0.0, 1.0)

    /*! \internal */
    function __updateMouseArea() {
        if (slider.mouseArea) {
            slider.mouseArea.pressedChanged.connect(__mouseAreaPressed);
            slider.mouseArea.positionChanged.connect(__mouseAreaPositionchanged);
        }
    }

    /*! \internal */
    function __mouseAreaPressed() {
        // FIXME: Instead of checking for the pressed button here, set acceptedButtons
        //  and only check for pressed.
        if (slider.mouseArea.pressedButtons == Qt.LeftButton) {
            // Left button pressed.
            var mouseX = slider.mouseArea.mouseX;
            var mouseY = slider.mouseArea.mouseY;
            if (mouseY >= thumbShape.y && mouseY <= thumbShape.y + thumbShape.height) {
                if (mouseX >= thumbShape.x && mouseX <= thumbShape.x + thumbShape.width) {
                    // Button pressed inside the thumb.
                    __dragInitMouseX = mouseX;
                    __dragInitNormalizedValue = __normalizedValue;
                    slider.pressed = true;
                } else if (mouseX > __thumbSpacing &&
                           mouseX < backgroundShape.width - __thumbSpacing) {
                    // Button pressed outside the thumb.
                    var normalizedPosition = (slider.mouseArea.mouseX - __thumbSpacing -
                    __thumbWidth * 0.5) / __thumbSpace;
                    normalizedPosition = MathUtils.clamp(normalizedPosition, 0.0, 1.0);
                    __value = MathUtils.lerp(normalizedPosition, slider.minimumValue,
                    slider.maximumValue);
                    __dragInitMouseX = mouseX;
                    __dragInitNormalizedValue = __normalizedValue;
                    slider.pressed = true;
                    if (slider.live) {
                        slider.value = __value
                    }
                }
            }
        } else {
            // Left button released.
            slider.pressed = false;
            if (!slider.live) {
                slider.value = __value;
            }
        }
    }

    /*! \internal */
    function __mouseAreaPositionchanged() {
        // Left button dragging.
        if (slider.pressed) {
            var normalizedOffsetX = (slider.mouseArea.mouseX - __dragInitMouseX) / __thumbSpace;
            var v = MathUtils.clamp(__dragInitNormalizedValue + normalizedOffsetX, 0.0, 1.0);
            __value = MathUtils.lerp(v, slider.minimumValue, slider.maximumValue);
            if (slider.live) {
                slider.value = __value
            }
        }
    }
}
