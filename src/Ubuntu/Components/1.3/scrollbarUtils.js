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
  \internal
  Object storing property names used in calculations.
  */
var _obj = {
    scrollbar: null,
    vertical: false,
    propOrigin: "",
    propContent: "",
    propPosRatio: "",
    propSizeRatio: "",
    propCoordinate: "",
    propSize: "",
    propContentLeadingMargin: "",
    propContentTrailingMargin: "",
    refresh: function () {
        _obj.vertical = (_obj.scrollbar.align === Qt.AlignLeading) || (_obj.scrollbar.align === Qt.AlignTrailing)
        _obj.propOrigin = (_obj.vertical) ? "originY" : "originX";
        _obj.propContent = (_obj.vertical) ? "contentY" : "contentX";
        _obj.propPosRatio = (_obj.vertical) ? "yPosition" : "xPosition";
        _obj.propSizeRatio = (_obj.vertical) ? "heightRatio" : "widthRatio";
        _obj.propCoordinate = (_obj.vertical) ? "y" : "x";
        _obj.propSize = (_obj.vertical) ? "height" : "width";
    }
}

/*!
  \internal
  Checks whether the _obj is valid or not. Must be called in every function
  as those can be invoked prior to the host (style) component completion.
  */
function __check(sb) {
    if (sb !== null && (_obj.scrollbar !== sb)) {
        _obj.scrollbar = sb;
        sb.flickableItemChanged.connect(_obj.refresh);
        sb.alignChanged.connect(_obj.refresh);
        _obj.refresh();
    }

    return _obj.scrollbar;
}

/*!
  Returns whether the scrollbar is vertical or horizontal.
  */
function isVertical(scrollbar) {
    if (!__check(scrollbar)) return 0;
    return _obj.vertical;
}

/*!
  Calculates the slider position based on the visible area's ratios.
  */
function sliderPos(scrollbar, min, max) {
    if (!__check(scrollbar)) return 0;
    //console.log("SLIDER POS", scrollbar.flickableItem.visibleArea[_obj.propPosRatio],"*", scrollbar.height, min, max)
    //the margin between the trough and the thumb min and max values
    var margin = scrollbar.__styleInstance.thumbsExtremesMargin

    //The total length of the path where the thumb can be positioned, from its min to its max value
    var draggableLength = scrollbar.__trough[_obj.propSize] - margin*2
    var maxPosRatio = 1.0 - scrollbar.flickableItem.visibleArea[_obj.propSizeRatio]

    //Example with x/width, same applies to y/height
    //xPosition is in the range [0...1 - widthRatio]
    //and we want the following mapping (xPosition --> thumb.x):
    //   0              ---> margin (margin is the min position for the thumb)
    //   1 - widthRatio ---> (draggableLength - scrollbar.__styleInstance.thumb[_obj.propSize]) + margin
    //So we scale the maximum thumb position by xPosition. But that's not enough, because that would mean
    //the maxPosition is reached when xPosition becomes 1, and that never happens. To compensate that, we
    //scale xPosition by ( 1 / ( 1 - widthRatio) ). This way, when xPosition reaches its max ( 1 - widthRatio )
    //we get a multiplication factor of 1
    return clamp(1.0 / maxPosRatio * scrollbar.flickableItem.visibleArea[_obj.propPosRatio]
                 * (draggableLength - scrollbar.__styleInstance.thumb[_obj.propSize]) + margin, min, max);
}

/*!
  Calculates the slider size for ListViews based on the visible area's position
  and size ratios, clamping it between min and max.

  The function can be used in Scrollbar styles to calculate the size of the slider.

  NOTE: THIS CODE IS ASSUMING THAT "MAX" IS ALSO THE "SIZE" OF THE TROUGH THAT THE
  THUMB CAN MOVE INTO! (which is what you want in 99.9% of the cases, for a scrollbar)
  */
function sliderSize(scrollbar, min, max) {
    if (!__check(scrollbar)) return 0;
    var sizeRatio = scrollbar.flickableItem.visibleArea[_obj.propSizeRatio];
    var posRatio = scrollbar.flickableItem.visibleArea[_obj.propPosRatio];

    //(sizeRatio * max) is the current ideal size, as recommended by Flickable visibleArea props
    var sizeUnderflow = (sizeRatio * max) < min ? min - (sizeRatio * max) : 0

    //we multiply by (max - sizeUndeflow) because we want to simulate a shorter trough. This is because
    //posRatio value is [0...1-sizeRatio] so it assumes the slider will be of size sizeRatio*size_of_the_trough
    //(because that's the only way to make the slider fill the remaining part of the trough when posRatio is
    //at its maximum value), while our slider could actually be bigger due to the imposed "min" value.
    //We will compensate for this shift by adding sizeUnderflow to endPos.
    var startPos = posRatio * (max - sizeUnderflow)
    var endPos = (posRatio + sizeRatio) * (max - sizeUnderflow) + sizeUnderflow
    var overshootStart = startPos < 0 ? -startPos : 0
    var overshootEnd = endPos > max ? endPos - max : 0

    // overshoot adjusted start and end
    var adjustedStartPos = startPos + overshootStart
    var adjustedEndPos = endPos - overshootStart - overshootEnd

    // final position and size of slider
    var position = adjustedStartPos + min > max ? max - min : adjustedStartPos
    var result = (adjustedEndPos - position) < min ? min : (adjustedEndPos - position)

    return result;
}

/*!
  The function calculates and clamps the position to be scrolled to the minimum
  and maximum values.

  The scroll and drag functions require a slider that does not have any minimum
  size set (meaning the minimum is set to 0.0). Implementations should consider
  using an invisible cursor to drag the slider and the ListView position.
  */
function scrollAndClamp(scrollbar, amount, min, max) {
    if (!__check(scrollbar)) return 0;
    console.log(scrollbar.flickableItem[_obj.propOrigin])
    console.log(scrollbar.flickableItem[_obj.propContent] - scrollbar.flickableItem[_obj.propOrigin] + amount, min, max)
    return scrollbar.flickableItem[_obj.propOrigin] +
            clamp(scrollbar.flickableItem[_obj.propContent] - scrollbar.flickableItem[_obj.propOrigin] + amount,
                  min, max);
}

/*!
  The function calculates the new position of the dragged slider. The amount is
  relative to the contentSize, which is either the flickable's contentHeight or
  contentWidth or other calculated value, depending on its orientation. The pageSize
  specifies the visibleArea, and it is usually the heigtht/width of the scrolling area.

  //FIXME: should we change the API and remove pageSize or just pass trough's size as pageSize par?

  */
function dragAndClamp(scrollbar, relThumbPosition, contentSize, pageSize) {
    if (!__check(scrollbar)) return 0;
    scrollbar.flickableItem[_obj.propContent] =
            scrollbar.flickableItem[_obj.propOrigin] + relThumbPosition * (contentSize - scrollbar.flickableItem[_obj.propSize]); //don't use pageSize, we don't know if the scrollbar is edge to edge!;
}

function scrollToBeginning(scrollbar) {
    if (!__check(scrollbar)) return 0;
    scrollbar.flickableItem[_obj.propContent] = 0
}

//FIXME: THIS DOESN'T TAKE THE CONTENT MARGINS (FLICKABLE.LEFTMARGIN ETC) INTO ACCOUNT!!
function scrollToEnd(scrollbar) {
    if (!__check(scrollbar)) return 0;
    scrollbar.flickableItem[_obj.propContent] = scrollbar.flickableItem.contentHeight - scrollbar.flickableItem[_obj.propSize]
}
