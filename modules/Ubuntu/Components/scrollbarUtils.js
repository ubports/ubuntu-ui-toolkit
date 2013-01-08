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

.pragma library

Qt.include("mathUtils.js")

/*!
  Checks whether the flickable is a ListView or not
  */
function isListView(flickable) {
    if (flickable && flickable.hasOwnProperty("header") && !flickable.hasOwnProperty("cellWidth")) {
        return true;
    }
    return false;
}

/*!
  Calculates the slider position based on the visible area's ratios.
  */
function sliderPos(vertical, flickable, scrollSize, min, max) {
    var posRatio = (vertical) ? "yPosition" : "xPosition";
    return clamp(flickable.visibleArea[posRatio] * scrollSize, min, max);
}

/*!
  Calculates the slider size for ListViews based on the visible area's position
  and size ratios, clamping it between min and max.

  The function can be used in Scrollbar delegates to calculate the size of the slider.
  */
function sliderSize(vertical, flickable, min, max) {
    var sizeRatio = (vertical) ? "heightRatio" : "widthRatio";
    var posRatio = (vertical) ? "yPosition" : "xPosition";
    var sizeUnderflow = (flickable.visibleArea[sizeRatio] * max) < min ? min - (flickable.visibleArea[sizeRatio] * max) : 0
    var startPos = flickable.visibleArea[posRatio] * (max - sizeUnderflow)
    var endPos = (flickable.visibleArea[posRatio] + flickable.visibleArea[sizeRatio]) * (max - sizeUnderflow) + sizeUnderflow
    var overshootStart = startPos < 0 ? -startPos : 0
    var overshootEnd = endPos > max ? endPos - max : 0

    // overshoot adjusted start and end
    var adjustedStartPos = startPos + overshootStart
    var adjustedEndPos = endPos - overshootStart - overshootEnd

    // final position and size of thumb
    var position = adjustedStartPos + min > max ? max - min : adjustedStartPos
    var result = (adjustedEndPos - position) < min ? min : (adjustedEndPos - position)

    return result;
}

/*!
  The function calculates and clamps the position to be scrolled to the minimum
  and maximum values.

  The scroll and drag functions require a slider that is not having any minimum
  size set (meaning the minimum is set to 0.0). Implementations should consider
  using an invisible cursor to drag the slider and the ListView position.
  */
function scrollAndClamp(amount, vertical, flickable, min, max) {
    var propOrigin = (vertical) ? "originY" : "originX";
    var propContent = (vertical) ? "contentY" : "contentX";
    return flickable[propOrigin] + clamp(flickable[propContent] - flickable[propOrigin] + amount, min, max);
}

/*!
  The function calculates the new position of the dragged slider. The amount is
  relative to the contentSize, which is either the flickable's contentHeight or
  contentWidth or other calculated value, depending on its orientation. The pageSize
  specifies the visibleArea, and it is usually the heigtht/width of the scrolling area.
  */
function dragAndClamp(cursor, vertical, flickable, contentSize, pageSize) {
    var propOrigin = (vertical) ? "originY" : "originX";
    var propContent = (vertical) ? "contentY" : "contentX";
    var propPos = (vertical) ? "y" : "x";
    flickable[propContent]  = flickable[propOrigin] + cursor[propPos] * contentSize / pageSize;
}
