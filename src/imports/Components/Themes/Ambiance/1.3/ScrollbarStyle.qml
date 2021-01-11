/*
 * Copyright 2015 Canonical Ltd.
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

/*
  The visuals handle both active and passive modes. This behavior is driven yet by
  the styledItem's __interactive property, however should be detected upon runtime based on
  the device type.
  On active scrollbars, positioning is handled so that the logic updates the flickable's
  X/Y content positions, which is then synched with the contentPosition by the main
  element.

  Style properties used:
    - interactive: bool - drives the interactive behavior of the scrollbar
    - minimumSliderSize: real - specifies the minimum size of the slider
    * overlay
        - overlay: bool - true if the scrollbar is overlay type
        - overlayOpacityWhenHidden: opacity when hidden
        - overlayOpacityWhenShown: opacity when shown
    * trough
        - troughThicknessIndicatorStyle: real - thickness of the thumb in Indicator style
        - troughThicknessThumbStyle: real - thickness of the thumb in Thumb style
        - troughThicknessSteppersStyle: real - thickness of the thumb in Steppers style
        - troughColorThumbStyle: color - color of the trough in Thumb style
        - troughColorSteppersStyle: color - color of the trough in Steppers style
    * slider
        - sliderColor: color - color for the slider
        - sliderRadius: real - radius of of the slider rectangle
        - thumbThickness: real - thickness of the slider in Thumb style
        - indicatorThickness: real - thickness of the slider in Indicator style
    * animations - where duration and easing properties are used only
        - scrollbarThicknessAnimation: PropertyAnimation - animation used when changing
                                       the thickness of the slider
        - scrollbarFadeInAnimation: PropertyAnimation - animation used when fading in
        - scrollbarFadeOutAnimation: PropertyAnimation - animation used when fading out
        - scrollbarFadeOutPause: int - milliseconds to pause before fading out
        - scrollbarCollapsePause: int - milliseconds to pause at the beginning of the
                                        transition from Thumb to Indicator style
    * scrolling UX - the values are relative to the size of the view
        - shortScrollingRatio: real - how much to scroll when using arrow keys and steppers
        - longScrollingRatio: real - how much to scroll when using PageUp/Down
    * other styling properties
        - hintingStyle: string - the style to use for the hinting feature
        - thumbsExtremesMargin: real - This is the top/bottom (for the vertical scrollbar)
                                       and left/right (for horiz scrollbar) margin of the slider
                                       from the trough.
                                       It is supposed to be the same margin as the one that
                                       separates the thumb from the left/right (or top/bottom
                                       for horizontal scrollbar) edges of the screen.
  */

Item {
    id: visuals

    property bool initialized: false

    /*****************************************************
     *      STYLING PROPERTIES                           *
     *****************************************************/
    property bool interactive: isMouseConnected || veryLongContentItem
    property real minimumSliderSize: __canFitSteppersAndTallerThumb
                                     ? __minSliderSizeWhenUnrestrictedSize
                                     : __minSliderSizeWhenRestrictedSize

    property bool overlay: !alwaysOnScrollbars
    property real overlayOpacityWhenShown: 1.0
    property real overlayOpacityWhenHidden: 0.0

    property real troughThicknessSteppersStyle : units.dp(14)
    property real troughThicknessThumbStyle : units.dp(14)
    property real troughThicknessIndicatorStyle : units.dp(9)
    property color troughColorThumbStyle: theme.palette.normal.foreground
    property color troughColorSteppersStyle: theme.palette.normal.foreground

    property color sliderColor: theme.palette.normal.overlayText

    //used for hover and pressed states, otherwise they're transparent in order to show the same
    //background as the trough
    property color stepperBgColor: theme.palette.normal.base
    property real sliderRadius: units.dp(3)
    property real thumbThickness: units.gu(1)
    property real indicatorThickness : units.dp(3)

    property PropertyAnimation scrollbarThicknessAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation scrollbarFadeInAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SlowDuration }
    property PropertyAnimation scrollbarFadeOutAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SlowDuration }
    property int scrollbarFadeOutPause: 3000
    property int scrollbarCollapsePause: 1000

    //The ratio of page to scroll when using pg_down/up
    //and arrow_down/up keyboard keys (and steppers)
    property real shortScrollingRatio: 0.1
    property real longScrollingRatio: 0.9

    property string hintingStyle: veryLongContentItem ? 'thumb' : 'indicator'
    property real thumbsExtremesMargin: units.dp(4)

    /*****************************************************
     *      HELPER PROPERTIES                            *
     *****************************************************/
    property alias thumb: slider
    property alias trough: trough

    //helper properties to ease code readability
    property bool isScrollable: styledItem.__private.scrollable && pageSize > 0.0
                                && contentSize > 0.0 && totalContentSize > pageSize
    property bool isVertical: (styledItem.align === Qt.AlignLeading) || (styledItem.align === Qt.AlignTrailing)
    property bool frontAligned: (styledItem.align === Qt.AlignLeading)
    property bool rearAligned: (styledItem.align === Qt.AlignTrailing)
    property bool topAligned: (styledItem.align === Qt.AlignTop)
    property bool bottomAligned: (styledItem.align === Qt.AlignBottom)

    //flickable helper properties
    //Don't do anything with the flickable until its Component.onCompleted is called, it's a waste of cycles
    property Flickable flickableItem: styledItem.__initializedFlickable
    property real pageSize: flickableItem
                            ? (isVertical ? flickableItem.height : flickableItem.width)
                            : 0
    property real contentSize: flickableItem
                               ? (isVertical ? flickableItem.contentHeight : flickableItem.contentWidth)
                               : 0
    property real leadingContentMargin: flickableItem
                                        ? (isVertical ? flickableItem.topMargin : flickableItem.leftMargin)
                                        : 0
    property real trailingContentMargin: flickableItem
                                         ? (isVertical ? flickableItem.bottomMargin : flickableItem.rightMargin)
                                         : 0
    //this size includes content margins
    property real totalContentSize: contentSize + leadingContentMargin + trailingContentMargin


    /*****************************************************
     *      INTERNAL PROPERTIES AND FUNCTIONS            *
     *****************************************************/

    //special handling of situations where we don't have enough room to show both thumb and steppers
    property bool __canFitSteppers: visuals[scrollbarUtils.propSize] >= 2*__idealStepperSize
    property bool __canFitShorterThumb: visuals[scrollbarUtils.propSize] >= (__minSliderSizeWhenRestrictedSize
                                                                             + 2*thumbsExtremesMargin)
    property bool __canFitSteppersAndShorterThumb: visuals[scrollbarUtils.propSize] >= (2*__idealStepperSize
                                                                                        + __minSliderSizeWhenRestrictedSize
                                                                                        + 2*thumbsExtremesMargin)
    property bool __canFitSteppersAndTallerThumb: visuals[scrollbarUtils.propSize] >= (2*__idealStepperSize
                                                                                       + __minSliderSizeWhenUnrestrictedSize
                                                                                       + 2*thumbsExtremesMargin)

    //the minimum slider size for common usecases, i.e. when there is enough space to show thumb + steppers
    property real __minSliderSizeWhenUnrestrictedSize: 3*thumbThickness
    property real __minSliderSizeWhenRestrictedSize: thumbThickness

    property real __targetStepperSize: !__canFitSteppers
                                       ? visuals[scrollbarUtils.propSize]/2
                                       : __idealStepperSize
    property real __idealStepperSize: troughThicknessSteppersStyle
    property real __stepperImgOpacityOnPressed: 1.0
    property real __stepperImgOpacityOnHover: 0.7
    property real __stepperImgOpacityNormal: 0.4
    property real __stepperImgOpacityDisabled: 0.2

    property real __stepperBgOpacityOnPressed: 1.0
    property real __stepperBgOpacityOnHover: 0.63

    //the stepper arrow svg is designed to scale nicely to the lowest
    //px-per-GU display (i.e. 8px/GU). Please consider updating the asset if
    //you change this size to avoid blurry steppers on low px-per-GU displays.
    property real __stepperAssetWidth: units.dp(8)

    //UX requested to disable this feature for the moment
    property bool __disableDragResetFeature: true

    property bool __recursionGuard: false
    property bool __disableStateBinding: false
    property alias __overshootTimer: overshootTimer
    property bool __hinting: false
    property bool draggingThumb: thumbArea.drag.active || slider.mouseDragging || slider.touchDragging
    //this is the condition that triggers "Thumb Style"
    //only show the thumb if the page AND the view is moving
    property bool thumbStyleFlag: veryLongContentItem && (flickableItem.moving || scrollAnimation.running)
    //we show thumb style instead of indicator style if the content item is very long on *any* of the 2 axes
    property bool veryLongContentItem: flickableItem && initialized
                                       && ((flickableItem.contentHeight > flickableItem.height * 10)
                                           || (flickableItem.contentWidth > flickableItem.width * 10))

    //this will eventually come from QInputInfo
    property bool isMouseConnected: true
    //used to offset viewport and scrollbars to make space for non-overlay scrollbars
    property real nonOverlayScrollbarMargin: troughThicknessSteppersStyle
    //TODO: move to Scrollbar.qml?
    property bool useSteppers: true
    //the thumb will have a touchDragStartMargin margin on both sides
    //to be more easily draggable via touch
    property real touchDragStartMargin: units.gu(2)
    //NOTE: this is not used for Qt's drag.treshold, just for our logic!
    property real dragThreshold: units.dp(2)
    //this could eventually become a system setting
    //True --> Steppers style, non-overlay scrollbars
    //False --> Indicator, Trough and Steppers styles, overlaid to the content
    property bool alwaysOnScrollbars: styledItem.__alwaysOnScrollbars

    function scrollBy(amount, animate) {
        if (isNaN(amount)) {
            console.log("BUG: Invalid scrolling delta.")
            return
        }
        scrollTo(scrollbarUtils.scrollAndClamp(
                     styledItem, amount, -leadingContentMargin,
                     Math.max(contentSize + trailingContentMargin - pageSize,
                              -leadingContentMargin))
                 , animate)
    }
    function scrollTo(value, animate) {
        if (isNaN(value)) {
            console.log("BUG: Invalid scrolling value.")
            return
        }
        //don't setup an animation with duration 0, if we just don't want the animation
        if (animate) {
            scrollAnimation.to = value
            scrollAnimation.restart()
        } else {
            if (scrollAnimation.running) scrollAnimation.stop()
            if (flickableItem) {
                flickableItem[scrollbarUtils.propContent] = value
                flickableItem.returnToBounds()
            }
        }
    }
    function scrollToBeginning(animate) {
        if (!flickableItem) return
        scrollTo(flickableItem[scrollbarUtils.propOrigin] - visuals.leadingContentMargin, animate)
    }
    function scrollToEnd(animate) {
        if (!flickableItem) return
        scrollTo((flickableItem[scrollbarUtils.propOrigin]
                  + totalContentSize - visuals.leadingContentMargin - pageSize), animate)
    }
    function drag() {
        scrollbarUtils.dragAndClamp(styledItem, slider.relThumbPosition, totalContentSize, pageSize);
    }
    function resetScrollingToPreDrag() {
        thumbArea.resetFlickableToPreDragState()
    }
    //As per spec, we show a hint of the scrollbars in indicator style whenever the
    //flickable item changes its size
    //NOTE: THIS IS ASSUMING THAT the transition to hidden state has a PauseAnimation,
    //otherwise the hinting would be too quick!
    function flashScrollbar() {
        //only show the hint if the scrollbar is currently hidden or its already showing the hint
        //(it could happen that while it is showing the hint the size of the flickable changes enough
        //to trigger the transition to thumb style, and in that case we want to hint again using thumb
        //style)
        if (initialized && isScrollable && !draggingThumb && !pressHoldTimer.running
                && (state == '' || state === 'hidden' || (__disableStateBinding && visuals.state !== hintingStyle))) {
            __disableStateBinding = true
            __hinting = true
            visuals.state = hintingStyle
            hintingTimer.restart()
        }
    }
    //The definition of isScrollable includes a binding on totalContentSize:
    //when totalContentSizeChanged triggers flashScrollbar, isScrollable may
    //have not been reevaluated yet using the new totalContentSize value, and
    //that could make the logic ignore the hinting request.
    //We perform the checks inside the flashScrollbar function instead of each
    //time before calling flashScrollbar (though this comes at slightly higher cost)
    onIsScrollableChanged: flashScrollbar()
    onTotalContentSizeChanged: flashScrollbar()
    onHintingStyleChanged: flashScrollbar()

    anchors.fill: parent
    opacity: overlayOpacityWhenHidden
    Component.onCompleted: initialized = true

    //we can't use onRunningChanged in the state transitions to enable the state binding again
    //after hinting, because that was causing
    //"Can't apply a state change as part of a state definition." when the state it was transitioning
    //to was different from the one the binding evaluationg was returning
    //Instead we use a timer, tune it so that it lasts as long as needed, and then we reenable the
    //state binding once the transition has completed
    Timer {
        id: hintingTimer
        repeat: false
        //we assume thickness animation duration is enough to complete the
        //transition to the hinting style
        interval: scrollbarThicknessAnimation.duration
        onTriggered: {
            __hinting = false
            __disableStateBinding = false
        }
    }

    /*!
        \internal
        Object storing property names used in calculations.
    */
    QtObject {
        id: scrollbarUtils
        objectName: "scrollbarUtils"
        property string propOrigin: (isVertical) ? "originY" : "originX"
        property string propContent: (isVertical) ? "contentY" : "contentX"
        property string propPosRatio: (isVertical) ? "yPosition" : "xPosition"
        property string propSizeRatio: (isVertical) ? "heightRatio" : "widthRatio"
        property string propCoordinate: (isVertical) ? "y" : "x"
        property string otherPropCoordinate: (isVertical) ? "x" : "y"
        property string propSize: (isVertical) ? "height" : "width"
        property string otherPropSize: (isVertical) ? "width" : "height"
        property string propAtBeginning: (isVertical) ? "atYBeginning" : "atXBeginning"
        property string propAtEnd: (isVertical) ? "atYEnd" : "atXEnd"

        /*!
          Calculates the slider position based on the visible area's ratios.
          */
        function sliderPos(scrollbar, min, max) {
            if (!scrollbar.__initializedFlickable) return min

            //the margin between the trough and the thumb min and max values
            var margin = scrollbar.__styleInstance.thumbsExtremesMargin

            //The total length of the path where the thumb can be positioned, from its min to its max value
            var draggableLength = visuals.trough[propSize] - margin*2
            var maxPosRatio = 1.0 - (scrollbar.__initializedFlickable ? scrollbar.__initializedFlickable.visibleArea[propSizeRatio] : 1.0)

            //Example with x/width, same applies to y/height
            //xPosition is in the range [0...1 - widthRatio]
            //and we want the following mapping (xPosition --> thumb.x):
            //   0              ---> margin (margin is the min position for the thumb)
            //   1 - widthRatio ---> (draggableLength - scrollbar.__styleInstance.thumb[propSize]) + margin
            //So we scale the maximum thumb position by xPosition. But that's not enough, because that would mean
            //the maxPosition is reached when xPosition becomes 1, and that never happens. To compensate that, we
            //scale xPosition by ( 1 / ( 1 - widthRatio) ). This way, when xPosition reaches its max ( 1 - widthRatio )
            //we get a multiplication factor of 1
            return MathUtils.clamp(1.0 / maxPosRatio * (scrollbar.__initializedFlickable ? scrollbar.__initializedFlickable.visibleArea[propPosRatio] : 1.0)
                                   * (draggableLength - scrollbar.__styleInstance.thumb[propSize]) + margin, min, max);
        }

        /*!
          Calculates the slider size for ListViews based on the visible area's position
          and size ratios, clamping it between min and max.

          The function can be used in Scrollbar styles to calculate the size of the slider.

          NOTE: THIS CODE IS ASSUMING THAT "MAX" IS ALSO THE "SIZE" OF THE TROUGH THAT THE
          THUMB CAN MOVE INTO! (which is what you want in 99.9% of the cases, for a scrollbar)
          */
        function sliderSize(scrollbar, min, max) {
            if (!scrollbar.__initializedFlickable) return min

            var sizeRatio = scrollbar.__initializedFlickable ? scrollbar.__initializedFlickable.visibleArea[propSizeRatio] : 1.0;
            var posRatio = scrollbar.__initializedFlickable ? scrollbar.__initializedFlickable.visibleArea[propPosRatio] : 0.0;

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
            if (!scrollbar.__initializedFlickable) return

            return scrollbar.__initializedFlickable[propOrigin] +
                    MathUtils.clamp(scrollbar.__initializedFlickable[propContent]
                                    - scrollbar.__initializedFlickable[propOrigin] + amount,
                                    min, max);
        }

        /*!
          The function calculates the new position of the dragged slider. The amount is
          relative to the contentSize, which is either the flickable's contentHeight or
          contentWidth or other calculated value, depending on its orientation. The pageSize
          specifies the visibleArea, and it is usually the heigtht/width of the scrolling area.

          //FIXME: should we change the API and remove pageSize or just pass trough's size as pageSize par?

          NOTE: when flickable.topMargin is 5GU, contentY has to be -5GU (not 0!) to be at the top of the scrollable!!
          */
        function dragAndClamp(scrollbar, relThumbPosition, contentSize, pageSize) {
            if (!scrollbar.__initializedFlickable) return

            scrollbar.__initializedFlickable[propContent] =
                    scrollbar.__initializedFlickable[propOrigin] + relThumbPosition * (contentSize - scrollbar.__initializedFlickable[propSize]) - leadingContentMargin; //don't use pageSize, we don't know if the scrollbar is edge to edge!;
        }
    }

    //each scrollbar connects to both width and height because
    //we want to show both the scrollbar in both cases
    Connections {
        target: (flickableItem && initialized) ? flickableItem : null
        onHeightChanged: flashScrollbar()
        onWidthChanged: flashScrollbar()
        //Use onTotalContentSizeChanged instead of binding to contentWidth/Height changes
        //to trigger the scrollbar hinting when the scrollable content is resized
    }

    SmoothedAnimation {
        id: scrollAnimation
        objectName: "scrollAnimation"

        //duration and easing coming from UX spec
        duration: UbuntuAnimation.SlowDuration
        easing.type: Easing.InOutCubic
        target: flickableItem
        property: scrollbarUtils.propContent
        //when the listview has variable size delegates the contentHeight estimation done by ListView
        //could make us overshoot, especially when going from top to bottom of the list or viceversa.
        //We call returnToBounds to mitigate that.
        onRunningChanged: if (!running && flickableItem) {
                              flickableItem.returnToBounds()
                          }
    }


    // The thumb appears whenever the mouse gets close enough to the scrollbar
    // and disappears after being for a long enough time far enough of it
    MouseArea {
        id: proximityArea

        //like containsMouse, but unlike it this property is not true when the user
        //taps on the area using touchscreen
        property bool containsMouseDevice: false

        anchors.fill: parent
        propagateComposedEvents: true
        preventStealing: false
        enabled: isScrollable && interactive
        hoverEnabled: isScrollable && interactive
        Mouse.ignoreSynthesizedEvents: true
        Mouse.onEntered: {
            isMouseConnected = true
            proximityArea.containsMouseDevice = true
        }
        Mouse.onExited: {
            overshootTimer.restart();
            proximityArea.containsMouseDevice = false
        }
        onPressed: { mouse.accepted = false }

        Timer {
            id: overshootTimer
            interval: scrollbarThicknessAnimation.duration * 10
        }

        // represents the visible area of the scrollbar where
        //slider and thumb connector are placed
        Rectangle {
            id: background
            anchors.fill: flowContainer
            objectName: "background"
            visible: flowContainer.showBackground
        }

        Flow {
            id: flowContainer
            property bool showSteppers: false
            property bool showBackground: false
            property bool showCornerRect: false
            //thickness of trough and thumb
            property real thickness: 0
            property real thumbThickness: 0

            property real proximityThickness: (isVertical) ? styledItem.width - thickness
                                                           : styledItem.height - thickness
            anchors {
                fill: parent
                leftMargin: (!isVertical || frontAligned) ? 0.0 : flowContainer.proximityThickness
                rightMargin: (!isVertical || rearAligned) ? 0.0 : flowContainer.proximityThickness
                topMargin: (isVertical || topAligned) ? 0.0 : flowContainer.proximityThickness
                bottomMargin: (isVertical || bottomAligned) ? 0.0 : flowContainer.proximityThickness
            }
            clip: true
            flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight

            Item {
                id: trough
                //account for 2 steppers at the end of the scrollbar
                width: isVertical ? parent.width
                                  : parent.width - steppersMouseArea.width
                height: isVertical ? parent.height - steppersMouseArea.height
                                   : parent.height
                objectName: "trough"

                Rectangle {
                    id: slider
                    objectName: "interactiveScrollbarThumb"

                    //this property is true if the user could be starting a drag (i.e. pressed inside the thumb)
                    //but drag.active is not true yet (because that requires dragging >0 pixels)
                    //Usecase: thumb is at the top, user drags towards the top and
                    //goes out of window -> drag.active won't trigger because the thumb can't move by even 1px,
                    //but we still want the logic to know that user could be trying to drag (by moving the
                    //mouse down again afterwards, for example)
                    //
                    //NOTE: starting a drag while the scrollbars are in steppers mode is considered a mouse drag
                    //(because we want the style to stay to steppers) even when started with touch!
                    property bool mouseDragging: false
                    property bool touchDragging: false

                    //the proportional position of the thumb relative to the trough it can move into
                    //It is 0 when the thumb is at its minimum value, and 1 when it is at its maximum value
                    property real relThumbPosition : isVertical
                                                     ? (slider.y - thumbsExtremesMargin) / (trough.height - 2*thumbsExtremesMargin - slider.height)
                                                     : (slider.x - thumbsExtremesMargin) / (trough.width - 2*thumbsExtremesMargin - slider.width)

                    //we just need to call this onPressed, so we shouldn't use a binding for it, which would get
                    //reevaluated any time one of the properties changes.
                    //+ having it as a binding has the sideeffect that when we query its value from inside onPressed
                    //it may not be using the most up-to-date value for *all* the variables it uses, and that would
                    //break the logic
                    function containsTouch() {
                        var touchX = slider.mapFromItem(thumbArea, thumbArea.mouseX, thumbArea.mouseY).x
                        var touchY = slider.mapFromItem(thumbArea, thumbArea.mouseX, thumbArea.mouseY).y
                        //relaxed checks on the position of the touch, to allow some error
                        if (touchX >= -visuals.touchDragStartMargin
                                && touchX <= slider.width + visuals.touchDragStartMargin
                                && touchY >= -visuals.touchDragStartMargin
                                && touchY <= slider.height + visuals.touchDragStartMargin) {
                            return true
                        } else {
                            return false
                        }
                    }

                    anchors {
                        verticalCenter: (isVertical) ? undefined : trough.verticalCenter
                        horizontalCenter: (isVertical) ? trough.horizontalCenter : undefined
                    }
                    x: (isVertical) ? 0 : scrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.width - slider.width - thumbsExtremesMargin)
                    y: (!isVertical) ? 0 : scrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.height - slider.height - thumbsExtremesMargin)
                    radius: visuals.sliderRadius
                    color: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b,
                                   sliderColor.a * (visuals.draggingThumb
                                                    ? 1.0
                                                    : (thumbArea.hoveringThumb ? 0.7 : 0.4 )))

                    //visible: HANDLED BY STATES

                    //This is to stop the scrollbar from changing size while being dragged when we have listviews
                    //with delegates of variable size (in those cases, contentWidth/height changes as the user scrolls
                    //because of the way ListView estimates the size of the out-of-views delegates
                    //and that would trigger resizing of the thumb)
                    //NOTE: otoh, we want the x/y binding to apply (even though it will cause a small flickering occasionally)
                    //even while dragging, because that guarantees the view is at the top when the user drags to the top
                    Binding {
                        when: !visuals.draggingThumb
                        target: slider
                        property: "width"
                        value: (isVertical) ? flowContainer.thumbThickness
                                            : scrollbarUtils.sliderSize(styledItem, visuals.minimumSliderSize, trough.width - 2 * thumbsExtremesMargin)
                    }
                    Binding {
                        when: !visuals.draggingThumb
                        target: slider
                        property: "height"
                        value: (!isVertical) ? flowContainer.thumbThickness
                                             : scrollbarUtils.sliderSize(styledItem, visuals.minimumSliderSize, trough.height - 2 * thumbsExtremesMargin)
                    }
                }

                //we reuse the MouseArea for touch interactions as well, because MultiTouchPointArea
                //can't propagate touch events underneath, if not consumed
                //(it would also require implementing drag as it doesn't have it)
                MouseArea {
                    id: thumbArea
                    objectName: "thumbArea"

                    property real originXAtDragStart: 0
                    property real originYAtDragStart: 0
                    property real contentXAtDragStart: 0
                    property real contentYAtDragStart: 0

                    //cache input device position in order to ignore mouse/touch
                    //moves which are shorter than a threshold. This is needed due
                    //to the low precision of some touchscreen devices, to avoid
                    //shaking the view while slowly dragging via touch
                    property real previousX: 0
                    property real previousY: 0


                    //THIS BEHAVIOUR IS CURRENTLY DISABLED, SEE __disableDragResetFeature
                    //following UX spec, if during a drag the mouse goes far away from the scrollbar
                    //we want to temporarily lock the drag and reset the scrollbar to where it was when
                    //the drag started, WITHOUT losing the drag (if the mouse gets back within the distance
                    //threshold, the drag has to resum without the need to release and pressHold again)
                    property bool lockDrag: false

                    property bool hoveringThumb: false

                    function initDrag(isMouse) {
                        __disableStateBinding = true
                        thumbArea.saveFlickableScrollingState()
                        if (isMouse) {
                            slider.mouseDragging = true
                            visuals.state = "steppers"
                        } else {
                            slider.touchDragging = true
                            visuals.state = "thumb"
                        }
                    }

                    function resetDrag() {
                        if (thumbArea.lockDrag) thumbArea.lockDrag = false
                        slider.mouseDragging = false
                        slider.touchDragging = false
                        __disableStateBinding = false
                    }

                    function handlePress(mouseX, mouseY) {
                        if (!thumbArea.pressed) return

                        var mouseScrollingProp = isVertical ? mouseY : mouseX
                        if (mouseScrollingProp < slider[scrollbarUtils.propCoordinate]) {
                            scrollBy(-pageSize*visuals.longScrollingRatio, true)
                        } else if (mouseScrollingProp > slider[scrollbarUtils.propCoordinate] + slider[scrollbarUtils.propSize]) {
                            scrollBy(pageSize*visuals.longScrollingRatio, true)
                        }
                    }

                    //we consider hover if it's inside the TROUGH along the scrolling axis
                    //and inside the THUMB along the non-scrolling axis
                    //NOTE: mouseX and mouseY are assumed to be relative to thumbArea
                    function handleHover(mouseX, mouseY) {
                        //NOTE: we're assuming thumbArea has same size/position as the trough.
                        //Use mapToItem to map the coordinates if that assumption falls (i.e. to implement
                        //a larger touch detection area around the thumb)
                        var mouseScrollingProp = isVertical ? mouseY : mouseX
                        var otherProp = isVertical ? mouseX : mouseY

                        //don't count as hover if the user is already press-and-holding
                        //the trough to scroll page by page
                        hoveringThumb = !(pressHoldTimer.running && pressHoldTimer.startedBy === thumbArea)
                                && mouseScrollingProp >= slider[scrollbarUtils.propCoordinate] &&
                                mouseScrollingProp <= slider[scrollbarUtils.propCoordinate] + slider[scrollbarUtils.propSize] &&
                                otherProp >= 0 && otherProp <= trough[scrollbarUtils.otherPropSize]
                    }

                    function saveFlickableScrollingState() {
                        originXAtDragStart = flickableItem.originX
                        originYAtDragStart = flickableItem.originY
                        contentXAtDragStart = flickableItem.contentX
                        contentYAtDragStart = flickableItem.contentY
                    }

                    function resetFlickableToPreDragState() {
                        //NOTE: when dealing with ListViews which have
                        //delegates that are highly variable in size originX/Y and contentWidth/Height
                        //change based on "out-of-view delegates" size estimation done by ListView.
                        //In that case, we can't do anything to bring back the view to where it was
                        //when the drag started, so we'd better not do anything instead of doing something
                        //which we're sure will be wrong
                        if (originXAtDragStart != flickableItem.originX
                                || originYAtDragStart != flickableItem.originY) {
                            return
                        }
                        flickableItem.contentX = contentXAtDragStart
                        flickableItem.contentY = contentYAtDragStart
                    }

                    function cacheMousePosition(mouse) {
                        previousX = mouse.x
                        previousY = mouse.y
                    }

                    //NOTE: remember to update the handleHover check if you add anchor margins
                    anchors.fill: trough

                    enabled: isScrollable && interactive && __canFitSteppersAndShorterThumb
                    onPressed: {
                        cacheMousePosition(mouse)
                        //potentially we allow using touch to trigger mouse interactions, in case the
                        //mouse has previously hovered over the area and activated the steppers style
                        //checking for the value of visuals.state wouldn't be useful here, the value could be
                        //"hidden" even if the values have only just started transitioning from "steppers" state

                        if (firstStepper.visible) {
                            var mouseScrollingProp = isVertical ? mouseY : mouseX
                            //don't start the press and hold timer to avoid conflicts with the drag
                            if (mouseScrollingProp < slider[scrollbarUtils.propCoordinate] ||
                                    mouseScrollingProp > (slider[scrollbarUtils.propCoordinate] + slider[scrollbarUtils.propSize])) {
                                handlePress(mouseX, mouseY)
                                pressHoldTimer.startTimer(thumbArea)
                            } else {
                                //we can't tell whether the drag is started from mouse or touch
                                //(unless we add an additional multipointtoucharea and reimplement drag
                                //but MPTArea can't ignore touch events so we wouldn't be able to propagate those)
                                //so we assume that it's a mouse drag if the mouse is within the proximity area
                                //when the mouse is dragged
                                initDrag(true)
                            }
                        } else if (visuals.veryLongContentItem && slider.containsTouch()) {
                            initDrag(false)
                        } else {
                            //propagate otherwise
                            mouse.accepted = false
                        }
                    }
                    onPositionChanged: {
                        //avoid shaking the view when the user is draggin via touch
                        if (Math.abs(mouse.x - previousX) < visuals.dragThreshold
                                && Math.abs(mouse.y - previousY) < visuals.dragThreshold) {
                            return
                        }
                        cacheMousePosition(mouse)

                        if (pressedButtons == Qt.LeftButton && (slider.mouseDragging || slider.touchDragging)) {
                            if (!__disableDragResetFeature &&
                                    ((isVertical && Math.abs(mouse.x - thumbArea.x) >= flowContainer.thickness * 10)
                                     || (!isVertical && Math.abs(mouse.y - thumbArea.y) >= flowContainer.thickness * 10))) {
                                //don't reset it if the user is already scrolling the view (via keyboard or similar)
                                if (!scrollAnimation.running) {
                                    resetFlickableToPreDragState()
                                    thumbArea.lockDrag = true
                                }
                            } else {
                                //don't clash with scrolling animation
                                if (scrollAnimation.running) scrollAnimation.stop()

                                if (thumbArea.lockDrag) thumbArea.lockDrag = false
                                visuals.drag()
                            }
                        }
                    }
                    onCanceled: {
                        hoveringThumb = false
                        resetDrag()
                        pressHoldTimer.stop()
                    }
                    onReleased: {
                        //don't call handleHover here as touch release also triggers this handler
                        //see bug #1616868
                        resetDrag()
                        pressHoldTimer.stop()
                    }
                    drag {
                        //don't start a drag while we're scrolling using press and hold
                        target: pressHoldTimer.running ? undefined : slider
                        axis: (isVertical) ? Drag.YAxis : Drag.XAxis
                        minimumY: lockDrag ? slider.y : thumbsExtremesMargin
                        maximumY: lockDrag ? slider.y : trough.height - slider.height - thumbsExtremesMargin
                        minimumX: lockDrag ? slider.x : thumbsExtremesMargin
                        maximumX: lockDrag ? slider.x : trough.width - slider.width - thumbsExtremesMargin
                        onActiveChanged: {
                            if (drag.active) {
                                //drag.active is true only when the target has been moved by >0 pixels (if threshold == 0)
                                //that means if the bar is at the top and user drags it upwards, drag.active won't change,
                                //but the user will still be able to leave the app window from above, move mouse to the left (or right)
                                //enough to trigger the drag lock logic, and then come back in the window and move downwards.
                                //At *that* point drag.active will become true, but we don't want to actually drag the thumb
                                //because the mouse is still outside the area where thumb is follow input device movements
                                if (!lockDrag) {
                                    visuals.drag()
                                }
                            } else {
                                resetDrag()
                            }
                        }

                        //NOTE: we need threshold to be 0, otherwise it will be
                        //impossible to drag contentItems which have a size so that
                        //"flickableItem.height - contentItem.height < threshold"!!!
                        threshold: 0
                    }

                    //logic to support different colour on hovering
                    hoverEnabled: true
                    //This means this mouse filter will only handle real mouse events!
                    //i.e. the synthesized mouse events created when you use
                    //touchscreen will not trigger it! This way we can have logic that
                    //will not trigger when using touch
                    Mouse.ignoreSynthesizedEvents: true
                    Mouse.enabled: true
                    Mouse.onEntered: {
                        hoveringThumb = false
                        handleHover(event.x, event.y)
                    }
                    Mouse.onPositionChanged: {
                        //We need to update the hover state also onPosChanged because this area
                        //covers the whole trough, not just the thumb, so entered/exited are not enough
                        //e.g. when mouse moves from inside the thumb to inside the trough, or when you
                        //click on the trough and the thumb scrolls and goes under the mouse cursor
                        handleHover(mouse.x, mouse.y)
                    }
                    Mouse.onExited: {
                        hoveringThumb = false
                    }
                    Mouse.onReleased: {
                        handleHover(mouse.x, mouse.y)
                    }

                    Timer {
                        id: pressHoldTimer

                        //This var is needed to reuse the same timer to handle
                        //both thumb and steppers press-and-hold
                        //NOTE: the item MUST provide a handlePress method
                        property MouseArea startedBy
                        property bool firstTrigger: true

                        function startTimer(item) {
                            startedBy = item
                            start()
                        }

                        interval: firstTrigger ? 500 : 250
                        onTriggered: {
                            if (firstTrigger) firstTrigger = false
                            if (startedBy !== undefined) {
                                startedBy.handlePress(startedBy.mouseX, startedBy.mouseY)
                            } else {
                                console.log("Congrats, you have just found a bug! Press and hold timer is running without knowing the item that started it. Please report this.")
                            }
                        }
                        repeat: true
                        onRunningChanged: {
                            if (!running) {
                                firstTrigger = true
                            }
                        }
                    }
                }
            }

            MouseArea {
                id: steppersMouseArea
                objectName: "steppersMouseArea"
                //size is handled by the states

                property bool hoveringFirstStepper: false
                property bool hoveringSecondStepper: false

                function handlePress() {
                    var mappedCoordFirstStepper = mapToItem(firstStepper, mouseX, mouseY)
                    var mappedCoordSecondStepper = mapToItem(secondStepper, mouseX, mouseY)

                    if (firstStepper.contains(Qt.point(mappedCoordFirstStepper.x, mappedCoordFirstStepper.y))) {
                        scrollBy(-pageSize * visuals.shortScrollingRatio, true)
                    } else if (secondStepper.contains(Qt.point(mappedCoordSecondStepper.x, mappedCoordSecondStepper.y))) {
                        scrollBy(pageSize * visuals.shortScrollingRatio, true)
                    }
                }

                function handleHover(event) {
                    var mappedCoordFirstStepper = mapToItem(firstStepper, event.x, event.y)
                    var mappedCoordSecondStepper = mapToItem(secondStepper, event.x, event.y)

                    if (firstStepper.contains(Qt.point(mappedCoordFirstStepper.x, mappedCoordFirstStepper.y))) {
                        hoveringFirstStepper = true
                        hoveringSecondStepper = false
                    } else if (secondStepper.contains(Qt.point(mappedCoordSecondStepper.x, mappedCoordSecondStepper.y))) {
                        hoveringFirstStepper = false
                        hoveringSecondStepper = true
                    } else {
                        hoveringFirstStepper = false
                        hoveringSecondStepper = false
                    }
                }

                enabled: isScrollable && interactive
                visible: flowContainer.showSteppers

                preventStealing: true
                //Logic that handles hovering on steppers
                hoverEnabled: true
                Mouse.enabled: true
                Mouse.ignoreSynthesizedEvents: true
                Mouse.onEntered: {
                    hoveringFirstStepper = false
                    hoveringSecondStepper = false
                    handleHover(event)
                }
                Mouse.onPositionChanged: {
                    handleHover(mouse)
                }
                Mouse.onExited: {
                    hoveringFirstStepper = false
                    hoveringSecondStepper = false
                }
                Mouse.onPressed: {
                    //This additional trigger of the hovering logic is useful especially in testing
                    //environments, where simulating a click on the first stepper will generate onEntered,
                    //but then clicking on the second one (without a mouseMove) won't, because they're
                    //handled by the same mouse area, which is already hovered. With this trigger
                    //we ensure that the hovering logic is correct even when there are no mouse moves between
                    //clicks on different steppers (like it happens in tst_actionSteppers test).
                    handleHover(mouse)
                }
                Mouse.onReleased: {
                    handleHover(mouse)
                }

                //We don't change the size of the images because we let the image reader figure the size out,
                //though that means we have to hide the images somehow while the mousearea is visible but has
                //null size. We choose to enable clipping here instead of creating bindings on images' visible prop
                clip: true
                onPressed: {
                    handlePress()
                    pressHoldTimer.startTimer(steppersMouseArea)
                }
                onReleased: pressHoldTimer.stop()
                onCanceled: pressHoldTimer.stop()

                Rectangle  {
                    id: firstStepper
                    objectName: "firstStepper"
                    anchors {
                        //it's left in both cases, otherwise using RTL
                        //would break the layout of the arrow
                        left: parent ? parent.left : undefined
                        right: isVertical ? (parent ? parent.right : undefined) : undefined
                        top: parent ? parent.top : undefined
                        bottom: !isVertical ? (parent ? parent.bottom : undefined) : undefined
                    }
                    color: (steppersMouseArea.hoveringFirstStepper && flickableItem && !flickableItem[scrollbarUtils.propAtBeginning])
                           ? Qt.rgba(stepperBgColor.r, stepperBgColor.g, stepperBgColor.b,
                                     stepperBgColor.a * (steppersMouseArea.pressed ? __stepperBgOpacityOnPressed : __stepperBgOpacityOnHover))
                           : "transparent"
                    visible: parent.visible
                    clip: true

                    Binding {
                        target: firstStepper
                        property: "height"
                        when: isVertical
                        value: visible ? __targetStepperSize : 0
                    }
                    Binding {
                        target: firstStepper
                        property: "width"
                        when: !isVertical
                        value: visible ? __targetStepperSize : 0
                    }
                    Icon {
                        objectName: "firstStepperIcon"
                        anchors.centerIn: parent
                        width: __stepperAssetWidth
                        rotation: isVertical ? 180 : 90
                        //NOTE: Qt.resolvedUrl was removed because it does not seem to be needed and
                        //the QML profiler showed it's relatively expensive
                        source: visible ? "../artwork/toolkit_scrollbar-stepper.svg" : ""
                        asynchronous: true
                        color: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b,
                                       sliderColor.a * ((flickableItem && flickableItem[scrollbarUtils.propAtBeginning])
                                                        ? __stepperImgOpacityDisabled
                                                        : (steppersMouseArea.hoveringFirstStepper
                                                           ? (steppersMouseArea.pressed ? __stepperImgOpacityOnPressed : __stepperImgOpacityOnHover)
                                                           : __stepperImgOpacityNormal)))
                    }
                }
                Rectangle {
                    id: secondStepper
                    objectName: "secondStepper"
                    anchors {
                        left: isVertical ? parent.left : firstStepper.right
                        right: isVertical ? parent.right : undefined
                        top: !isVertical ? parent.top : firstStepper.bottom
                        bottom: !isVertical ? parent.bottom : undefined
                    }
                    color: (steppersMouseArea.hoveringSecondStepper && flickableItem && !flickableItem[scrollbarUtils.propAtEnd])
                           ? Qt.rgba(stepperBgColor.r, stepperBgColor.g, stepperBgColor.b,
                                     stepperBgColor.a * (steppersMouseArea.pressed ? __stepperBgOpacityOnPressed : __stepperBgOpacityOnHover))
                           : "transparent"
                    clip: true
                    visible: parent.visible

                    Binding {
                        target: secondStepper
                        property: "height"
                        when: isVertical
                        value: visible ? __targetStepperSize : 0
                    }
                    Binding {
                        target: secondStepper
                        property: "width"
                        when: !isVertical
                        value: visible ? __targetStepperSize : 0
                    }
                    Icon {
                        id: icon
                        objectName: "secondStepperIcon"
                        anchors.centerIn: parent
                        width: __stepperAssetWidth
                        rotation: isVertical ? 0 : -90
                        //NOTE: Qt.resolvedUrl was removed because it does not seem to be needed and
                        //the QML profiler showed it's relatively expensive
                        source: visible ? "../artwork/toolkit_scrollbar-stepper.svg" : ""
                        asynchronous: true
                        color: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b,
                                       sliderColor.a * ((flickableItem && flickableItem[scrollbarUtils.propAtEnd])
                                                        ? __stepperImgOpacityDisabled
                                                        : (steppersMouseArea.hoveringSecondStepper
                                                           ? (steppersMouseArea.pressed ? __stepperImgOpacityOnPressed : __stepperImgOpacityOnHover)
                                                           : __stepperImgOpacityNormal)))
                    }
                }
            }
        }

        //just a hack: a rectangle which covers the corner where scrollbars meet, when they're in steppers style
        Rectangle {
            id: cornerRect
            //NOTE: I tried grouping styledItem.__buddyScrollbar && styledItem.__buddyScrollbar.__styleInstance
            //    && styledItem.__buddyScrollbar.__styleInstance.isScrollable in 1 property and use that for
            //all the anchors, but it turned out it makes the instantiation 2-3% slower!!!
            anchors.left: {
                if (styledItem.__buddyScrollbar && styledItem.__buddyScrollbar.__styleInstance
                        && styledItem.__buddyScrollbar.__styleInstance.isScrollable) {
                    if (isVertical) {
                        return flowContainer.left
                    } else {
                        if (styledItem.__buddyScrollbar.align === Qt.AlignTrailing) {
                            return flowContainer.right
                        } else
                            return undefined
                    }
                }
            }
            anchors.right: {
                if (styledItem.__buddyScrollbar && styledItem.__buddyScrollbar.__styleInstance
                        && styledItem.__buddyScrollbar.__styleInstance.isScrollable) {
                    if (isVertical) {
                        return flowContainer.right
                    } else {
                        if (styledItem.__buddyScrollbar.align === Qt.AlignLeading) {
                            return flowContainer.left
                        } else {
                            return undefined
                        }
                    }
                }
            }
            anchors.top: {
                if (styledItem.__buddyScrollbar && styledItem.__buddyScrollbar.__styleInstance
                        && styledItem.__buddyScrollbar.__styleInstance.isScrollable) {
                    if (isVertical) {
                        if (styledItem.__buddyScrollbar.align === Qt.AlignBottom) {
                            return flowContainer.bottom
                        } else {
                            return undefined
                        }
                    } else {
                        return undefined
                    }
                }
            }
            anchors.bottom: {
                if (styledItem.__buddyScrollbar && styledItem.__buddyScrollbar.__styleInstance
                        && styledItem.__buddyScrollbar.__styleInstance.isScrollable) {
                    if (isVertical) {
                        if (styledItem.__buddyScrollbar.align === Qt.AlignTop) {
                            return flowContainer.top
                        } else {
                            return undefined
                        }
                    } else {
                        return flowContainer.bottom
                    }
                }
            }
            //REMEMBER: the thickness animates during state changes,
            //so we want to bind the right side to the thickness of the scrollbar
            width: isVertical ? flowContainer.thickness : troughThicknessSteppersStyle
            height: isVertical ? troughThicknessSteppersStyle : flowContainer.thickness
            color: background.color
            visible: flowContainer.showCornerRect
                     && styledItem.__buddyScrollbar
                     && styledItem.__buddyScrollbar.__styleInstance.isScrollable
        }
    }

    Binding {
        when: !__disableStateBinding
        target: visuals
        property: 'state'
        value: {
            if (!isScrollable && !alwaysOnScrollbars) {
                return '';
            }
            else if (overlay) {
                //we use the check on running to avoid to make it so that the scrollbar completes the transition
                //to the steppers (or thumb) state even when the mouse exits the area prematurely (e.g. when the mouse
                //overshoots, without the .running check the scrollbar would stop the growing transition halfway and
                //go back to hidden mode)
                // NOTE: we use steppersMouseArea.pressed to keep steppers state even when the user presses on the steppers
                // then holds and moves the pointer outside the window (probably by mistake) without releasing
                if (proximityArea.containsMouseDevice || steppersMouseArea.pressed
                        //if the user is press-and-holding on the trough to scroll page by page
                        || (pressHoldTimer.running && pressHoldTimer.startedBy === thumbArea)
                        || (draggingThumb && slider.mouseDragging) || __overshootTimer.running) {
                    return 'steppers'
                } else if (thumbStyleFlag
                           || (draggingThumb && slider.touchDragging) || __overshootTimer.running) {
                    return 'thumb'
                } else if ((flickableItem && flickableItem.moving) || scrollAnimation.running || transitionToIndicator.running) {
                    return 'indicator';
                } else return 'hidden';
            } else {
                return 'steppers'
            }
        }
    }

    states: [
        State {
            name: 'hidden'
            extend: ''
            PropertyChanges {
                target: visuals
                opacity: overlayOpacityWhenHidden
            }
        },
        State {
            name: 'shown'
            PropertyChanges {
                target: visuals
                opacity: overlayOpacityWhenShown
            }
            PropertyChanges {
                target: slider
                visible: __canFitShorterThumb
            }
        },
        State {
            name: 'indicator'
            extend: 'shown'
            PropertyChanges {
                target: flowContainer
                thickness: troughThicknessIndicatorStyle
                thumbThickness: indicatorThickness
                showSteppers: false
                showBackground: alwaysOnScrollbars
                showCornerRect: false
            }
            PropertyChanges {
                target: steppersMouseArea
                width: 0
                height: 0
            }
            PropertyChanges {
                target: background
                color: Qt.rgba(troughColorThumbStyle.r,
                               troughColorThumbStyle.g,
                               troughColorThumbStyle.b,
                               troughColorThumbStyle.a * 0.2)
            }
        },
        State {
            name: 'thumb'
            extend: 'shown'
            PropertyChanges {
                target: flowContainer
                thickness: visuals.troughThicknessThumbStyle
                thumbThickness: visuals.thumbThickness
                showSteppers: false
                showBackground: alwaysOnScrollbars
                showCornerRect: false
            }
            PropertyChanges {
                target: steppersMouseArea
                width: 0
                height: 0
            }
            PropertyChanges {
                target: background
                color: Qt.rgba(troughColorThumbStyle.r,
                               troughColorThumbStyle.g,
                               troughColorThumbStyle.b,
                               troughColorThumbStyle.a * 0.2)
            }
        },
        State {
            name: 'steppers'
            extend: 'shown'
            PropertyChanges {
                target: flowContainer
                thickness: visuals.troughThicknessSteppersStyle
                thumbThickness: visuals.thumbThickness
                showSteppers: visuals.useSteppers
                showBackground: true
                showCornerRect: true
            }
            PropertyChanges {
                target: steppersMouseArea
                width: isVertical ? flowContainer.width
                                  : (firstStepper.visible ? firstStepper.width : 0)
                                    + (secondStepper.visible ? secondStepper.width : 0)
                height: isVertical ? (firstStepper.visible ? firstStepper.height : 0)
                                     + (secondStepper.visible ? secondStepper.height : 0)
                                   : trough.height
            }
            PropertyChanges {
                target: background
                color: Qt.rgba(troughColorSteppersStyle.r,
                               troughColorSteppersStyle.g,
                               troughColorSteppersStyle.b,
                               troughColorSteppersStyle.a * 0.9)
            }
            PropertyChanges {
                target: slider
                visible: __canFitSteppersAndShorterThumb
            }
        }
    ]
    transitions: [
        Transition {
            from: ''
            to: 'shown'
            NumberAnimation {
                target: visuals
                property: "opacity"
                duration: scrollbarFadeInAnimation.duration
                easing: scrollbarFadeInAnimation.easing
            }
        },
        Transition {
            from: 'thumb,steppers'
            to: 'indicator'
            id: collapsingTransition
            SequentialAnimation {
                //don't pause the animation if we're showing the *hint* and the hintingStyle changes from
                //thumb/steppers to indicator
                PauseAnimation { id: scrollbarCollapseAnim; duration: __disableStateBinding ? 0 : scrollbarCollapsePause }
                PropertyAction {
                    target: steppersMouseArea
                    properties: isVertical ? "width" : "height"
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: flowContainer
                        properties: "thickness,thumbThickness"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: steppersMouseArea
                        properties: isVertical ? "height" : "width"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    ColorAnimation { target: background; duration: scrollbarThicknessAnimation.duration }
                }
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showBackground,showCornerRect"
                }
                PropertyAction {
                    target: slider
                    properties: "visible"
                }
            }
        },
        Transition {
            id: transitionToIndicator
            from: '*'
            to: 'indicator'
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: visuals
                        property: "opacity"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: flowContainer
                        properties: "thickness,thumbThickness"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    ColorAnimation { target: background; duration: scrollbarThicknessAnimation.duration }
                }
                //this covers the case when scrollbar is still transitioning from steppers style to hidden,
                //but the transition is not yet finished, and we flick the view. At that point the state is "hidden"
                //but the properties are still transitioning from steppers style to hidden. If a change to "indicator"
                //style is triggered while the properties are still transitioning, we have to handle those properties
                //as well to avoid immediate changes in the UI.
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showBackground,showCornerRect"
                }
                PropertyAction {
                    target: slider
                    properties: "visible"
                }
            }
        },
        Transition {
            id: growingTransition
            objectName: "indicatorToThumbSteppersTransition"
            from: 'indicator'
            to: 'thumb,steppers'
            reversible: true
            SequentialAnimation {
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showBackground,showCornerRect"
                }
                PropertyAction {
                    target: steppersMouseArea
                    properties: isVertical ? "width" : "height"
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: visuals
                        property: "opacity"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: flowContainer
                        properties: "thickness,thumbThickness"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: steppersMouseArea
                        properties: isVertical ? "height" : "width"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    ColorAnimation { target: background; duration: scrollbarThicknessAnimation.duration }
                }
                PropertyAction {
                    target: slider
                    properties: "visible"
                }
            }
        },
        Transition {
            id: steppersTransition
            objectName: "anythingToThumbSteppersTransition"
            from: '*'
            to: 'thumb,steppers'
            SequentialAnimation {
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showBackground,showCornerRect"
                }
                PropertyAction {
                    target: steppersMouseArea
                    properties: isVertical ? "width" : "height"
                }
                //hide the thumb (if needed) before the steppers expand (otherwise you'd see the
                //thumb sliding out of screen before hiding)
                PropertyAction {
                    target: slider
                    properties: "visible"
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: visuals
                        property: "opacity"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: flowContainer
                        properties: "thickness,thumbThickness"
                        //the check on opacity is needed because this transition is also run when a transition to "hidden"
                        //is still in progress! For example: transition from indicator to hidden is started, the state is set to "hidden"
                        //immediately, but the transition still has to happen. If, in this situation, we trigger a state change from "hidden"
                        //to another state (thumb/steppers) this transition is fired, but the fact that state is "hidden" is not
                        //really informative, we need to know if the stuff is actually visible on screen, because we want different
                        //animation based on whether we're animating from visible or invisible content, even if in both cases the state is "hidden"!
                        duration: visuals.opacity > 0 ? scrollbarThicknessAnimation.duration : 0
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: steppersMouseArea
                        properties: isVertical ? "height" : "width"
                        duration: visuals.opacity > 0 ? scrollbarThicknessAnimation.duration : 0
                        easing: scrollbarThicknessAnimation.easing
                    }
                    ColorAnimation { target: background; duration: visuals.opacity > 0 ? scrollbarThicknessAnimation.duration : 0 }
                }
            }
        },
        Transition {
            id: hidingAnimation
            from: '*'
            to: 'hidden'
            SequentialAnimation {
                PauseAnimation { id: fadeOutPauseAnim; duration: scrollbarFadeOutPause }
                NumberAnimation {
                    target: visuals
                    property: "opacity"
                    duration: scrollbarFadeOutAnimation.duration
                    easing: scrollbarFadeOutAnimation.easing
                }
                PropertyAction {
                    target: steppersMouseArea
                    properties: "width,height"
                }
                PropertyAction {
                    target: flowContainer
                    properties: "thickness,thumbThickness,showSteppers,showBackground,showCornerRect"
                }
                PropertyAction {
                    target: background
                    properties: "color"
                }
                PropertyAction {
                    target: slider
                    properties: "visible"
                }
                PropertyAction {
                    target: visuals
                    properties: "state"
                }
            }
        }
    ]
}
