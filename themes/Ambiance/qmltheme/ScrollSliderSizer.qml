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

import QtQuick 2.0

/*!
  \internal
  \qmltype ScrollSliderSizer
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup ubuntu
  \brief Scrollbar Decorator (Slider) sizer object, provides sizing and elasticity
  support for the slider
  */
QtObject {
    // relative (0..1) position of top and bottom
    property real positionRatio
    property real sizeRatio

    // max position and min size
    property real maximumPosition
    property real minimumSize

    // size underflow
    property real sizeUnderflow: (sizeRatio * maximumPosition) < minimumSize ? minimumSize - (sizeRatio * maximumPosition) : 0

    // raw start and end position considering minimum size
    property real rawStartPosition: positionRatio * (maximumPosition - sizeUnderflow)
    property real rawEndPosition: (positionRatio + sizeRatio) * (maximumPosition - sizeUnderflow) + sizeUnderflow

    // overshoot amount at start and end
    property real overshootStart: rawStartPosition < 0 ? -rawStartPosition : 0
    property real overshootEnd: rawEndPosition > maximumPosition ? rawEndPosition - maximumPosition : 0

    // overshoot adjusted start and end
    property real adjustedStartPosition: rawStartPosition + overshootStart
    property real adjustedEndPosition: rawEndPosition - overshootStart - overshootEnd

    // final position and size of thumb
    property int position: 0.5 + (adjustedStartPosition + minimumSize > maximumPosition ? maximumPosition - minimumSize : adjustedStartPosition)
    property int size: 0.5 + ((adjustedEndPosition - position) < minimumSize ? minimumSize : (adjustedEndPosition - position))
}

