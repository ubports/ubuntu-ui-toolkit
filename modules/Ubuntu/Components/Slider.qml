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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import "." 0.1 as Theming

/*!
    \qmltype Slider
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
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

    width: units.gu(38)
    height: units.gu(4)

    Theming.ItemStyle.class: "slider"

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
      This function is used by the value indicator to show the current value.
      Reimplement this function if you want to show different information. By
      default, the value v is rounded to the nearest interger value.
     */
    function formatValue(v) {
        return v.toFixed(0)
    }

    // Private symbols.
    
    /*!
      \internal
      Slider value to be updated live
    */
    property real __liveValue: 0.0
    /*! \internal */
    onValueChanged: __liveValue = slider.value

    /*!
      \internal
      Should be used by the delegates to position the thumbs.
    */
    property real __normalizedValue: MathUtils.clamp((__liveValue - slider.minimumValue) /
                                                     (slider.maximumValue - slider.minimumValue),
                                                     0.0, 1.0)

    /*!
      \internal
      Should be set by the delegates
    */
    property Item __thumb

    /*!
      \internal
      Should be set by the delegates
    */
    property Item __background

    /*! \internal */
    Component.onCompleted: internals.updateMouseArea()

    /*! \internal */
    onPressedChanged: internals.mouseAreaPressed()

    QtObject {
        id: internals

        property real dragInitMouseX: 0.0
        property real dragInitNormalizedValue: 0.0
        property real thumbSpacing: ComponentUtils.style(slider, "thumbSpacing", units.dp(2))
        property real thumbWidth: slider.height - thumbSpacing
        property real thumbSpace: __background.width - (2.0 * thumbSpacing + thumbWidth)

        function updateMouseArea() {
            slider.__mouseArea.positionChanged.connect(internals.mouseAreaPositionchanged);
        }

        function mouseAreaPressed() {
            if (slider.__mouseArea.pressedButtons == Qt.LeftButton) {
                // Left button pressed.
                var mouseX = slider.__mouseArea.mouseX;
                var mouseY = slider.__mouseArea.mouseY;
                if (mouseY >= __thumb.y && mouseY <= __thumb.y + __thumb.height) {
                    if (mouseX >= __thumb.x && mouseX <= __thumb.x + __thumb.width) {
                        // Button pressed inside the thumb.
                        dragInitMouseX = mouseX;
                        dragInitNormalizedValue = __normalizedValue;
                    } else if (mouseX > thumbSpacing &&
                               mouseX < __background.width - thumbSpacing) {
                        // Button pressed outside the thumb.
                        var normalizedPosition = (slider.__mouseArea.mouseX - thumbSpacing - thumbWidth * 0.5) / thumbSpace;
                        normalizedPosition = MathUtils.clamp(normalizedPosition, 0.0, 1.0);
                        __liveValue = MathUtils.lerp(normalizedPosition, slider.minimumValue, slider.maximumValue);
                        dragInitMouseX = mouseX;
                        dragInitNormalizedValue = __normalizedValue;
                        if (slider.live) {
                            slider.value = __liveValue;
                        }
                    }
                }
            } else {
                // Button released.
                if (!slider.live) {
                    slider.value = __liveValue;
                }
            }
        }

        function mouseAreaPositionchanged() {
            // Left button dragging.
            if (slider.pressed) {
                var normalizedOffsetX = (slider.__mouseArea.mouseX - dragInitMouseX) / thumbSpace;
                var v = MathUtils.clamp(dragInitNormalizedValue + normalizedOffsetX, 0.0, 1.0);
                __liveValue = MathUtils.lerp(v, slider.minimumValue, slider.maximumValue);
                if (slider.live) {
                    slider.value = __liveValue;
                }
            }
        }
    }
}
