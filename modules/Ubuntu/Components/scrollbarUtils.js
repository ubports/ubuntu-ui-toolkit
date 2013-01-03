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

// .pragma library FIXME cannot use units

function isListView(flickableItem) {
    if (flickableItem && flickableItem.hasOwnProperty("header") && !flickableItem.hasOwnProperty("cellWidth")) {
        return true;
    }
    return false;
}

/*!
  The function returns the minimum clamp value needed for the flickableItem. The
  value depends on whether the flickableItem si a ListView and if the listView has
  header or not.
  */

function clampMin(vertical, flickableItem, defaultValue) {
    var result = defaultValue;
    if (isListView(flickableItem)) {
        var sizeProp = (vertical) ? 'height' : 'width'
        if (flickableItem.header)
            result -= flickableItem.headerItem[sizeProp];
    }
    return result;
}

/*!
  The function returns the maximum clamp value needed for the flickableItem. The
  value depends on whether the flickableItem is a ListView and if the ListView has
  footer or not.
  */
function clampMax(vertical, flickableItem, defaultValue) {
    var result = defaultValue;
    if (isListView(flickableItem)) {
        var sizeProp = (vertical) ? 'height' : 'width'
        if (flickableItem.footer)
            result -= flickableItem.footerItem[sizeProp];
    }
    return result;
}

/*!
  Calculates the slider position based on the visible area's ratios.
  */
function sliderPos(vertical, flickableItem, itemSize, min, max) {
    var posRatio = (vertical) ? "yPosition" : "xPosition";
    return MathUtils.clamp(flickableItem.visibleArea[posRatio] * itemSize, min, max);
}

/*!
  Calculates the slider size for ListViews based on the visible area's position
  and size ratios, clamping it between min and max.

  The function can be used in Scrollbar delegates to calculate the size of the slider.
  */
function sliderSize(vertical, flickableItem, min, max) {
    var sizeRatio = (vertical) ? "heightRatio" : "widthRatio";
    var posRatio = (vertical) ? "yPosition" : "xPosition";
    var sizeUnderflow = (flickableItem.visibleArea[sizeRatio] * max) < min ? min - (flickableItem.visibleArea[sizeRatio] * max) : 0
    var startPos = flickableItem.visibleArea[posRatio] * (max - sizeUnderflow)
    var endPos = (flickableItem.visibleArea[posRatio] + flickableItem.visibleArea[sizeRatio]) * (max - sizeUnderflow) + sizeUnderflow
    var overshootStart = startPos < 0 ? -startPos : 0
    var overshootEnd = endPos > max ? endPos - max : 0

    // overshoot adjusted start and end
    var adjustedStartPos = startPos + overshootStart
    var adjustedEndPos = endPos - overshootStart - overshootEnd

    // final position and size of thumb
    var position = adjustedStartPos + min > max ? max - min : adjustedStartPos
    var size = (adjustedEndPos - position) < min ? min : (adjustedEndPos - position)
    return size;
}

/*!
  The function calculates the position to be scrolled to depending on whether the
  flickableItem is a ListView or other Flickable.
  */
function scrollToPosition(vertical, position, flickableItem, itemSize, contentSize, pageSize) {
    var min = 0.0;
    var max = contentSize - pageSize;
    var origin = (vertical) ? flickableItem.originY : flickableItem.originX;
    if (isListView(flickableItem)) {
        var sizeProperty = (vertical) ? "height" : "width";
        min = flickableItem.header ? -flickableItem.headerItem[sizeProperty] : 0.0;
        if (flickableItem.header && flickableItem.section.delegate)
            max -= itemSize;
    }
    return MathUtils.clamp(position + origin, min, max);
}

/*!
  The function calculates the new position of the slider scrolled by the given
  amount. The amount is relative to the contentSize, which in order specifies
  the total scrolling amount of th eflickableItem in pixels. The slider specifies
  the QML component visualizing the slider, and the pageSize specifies the visibleArea
  */
function scrollSliderBy(vertical, amount, flickableItem, itemSize, slider, contentSize, pageSize) {
    var sizeProp = (vertical) ? "height" : "width";
    var posProp = (vertical)  ? "y" : "x";
    var sign = (amount >= 0) ? 1 : -1;
    amount = sign * MathUtils.projectValue(Math.abs(amount), 0.0, contentSize - pageSize, 0.0, flickableItem[sizeProp] - slider[sizeProp]);
    var pos = Math.ceil(slider[posProp] + amount);
    pos = MathUtils.clampAndProject(pos, 0.0, flickableItem[sizeProp] - slider[sizeProp], 0.0, contentSize - pageSize);
    return scrollToPosition(vertical, pos, flickableItem, itemSize, contentSize, pageSize);
}
