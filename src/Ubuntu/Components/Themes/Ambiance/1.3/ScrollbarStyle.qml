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
    * animations - where duration and easing properties are used only
        - scrollbarFadeInAnimation: PropertyAnimation - animation used when fade in
        - scrollbarFadeOutAnimation: PropertyAnimation - animation used when fade out
        - scrollbarFadeOutPause: int - miliseconds to pause before fade out
    * behaviors - animations are used as declared
        - sliderAnimation: PropertyAnimation - animation for the slider size
        - thumbConnectorFading: PropertyAnimation - animation for the thumb connector
    * other styling properties
        - color sliderColor: color for the slider
        - real scrollAreaThickness: scrollbar area thickness, the area where the
                                    slider, thumb and thumb-connector appear
  */

Item {
    id: visuals
    // styling properties
    property bool interactive: false
    property real minimumSliderSize: units.gu(2)

    property bool overlay: !alwaysOnScrollbars
    property real overlayOpacityWhenShown: 1.0
    property real overlayOpacityWhenHidden: 0.0

    property real troughThicknessSteppersStyle : units.gu(2)
    property real troughThicknessThumbStyle : units.gu(2)
    property real troughThicknessIndicatorStyle : units.dp(12) //1.5gu

    property real shortScrollingRatio: 0.1
    property real longScrollingRatio: 0.9

    //used to offset viewport and scrollbars to allow non-overlay scrollbars
    property real nonOverlayScrollbarMargin: troughThicknessSteppersStyle

    //whether we should show steppers or not. This has to be linked to a gsetting or something like that
    //because it will probably be a system setting
    //TODO: move to Scrollbar.qml?
    property bool useSteppers: true

    //INTERNAL: simulate the system setting (which will be implemented in unity8, I guess)
    //True --> Steppers style, non-overlay scrollbars
    //False --> Indicator and Trough styles
    property bool alwaysOnScrollbars: styledItem.__alwaysOnScrollbars

    property string hintingStyle: veryLongContentItem ? 'thumb' : 'indicator'
    onHintingStyleChanged: flashScrollbar()

    //we show thumb style instead of indicator style if the content item is very long on *any* of the 2 axes
    property bool veryLongContentItem: {
        console.log("EVALUATING veryLongContentItem for item", visuals, isVertical, flickableItem, initialized,
                    flickableItem.contentHeight, flickableItem.height, (flickableItem.contentHeight > flickableItem.height * 10),
                    flickableItem.contentWidth, flickableItem.width, (flickableItem.contentWidth > flickableItem.width * 10))
        return flickableItem && initialized
                && ((flickableItem.contentHeight > flickableItem.height * 10)
                    || (flickableItem.contentWidth > flickableItem.width * 10))
    }

    property real thumbThickness: units.gu(1)
    property real indicatorThickness : units.dp(4)
    //this is the top/bottom (for the vertical scrollbar) and left/right (for horiz scrollbar) margin
    //from the trough
    //It is supposed to be the same margin as the one that separates the thumb from the left/right
    //(or top/bottom for horizontal scrollbar) edge of the screen
    property real thumbsExtremesMargin: units.dp(4)
    Component.onCompleted: initialized = true
    property bool initialized: false

    //this is the flag that tells us if we should change style when the mouse is hovering on the proximity area
    property bool hoverTransformationFlag: (proximityArea.containsMouse /*&& (flickableItem.moving || scrollAnimation.running)*/)
    //this is the condition that triggers "Thumb Style"
    //This property is also queried by the "buddyScrollbar" to make sure both scrollbars
    //are in thumb style even when only one of them has a true thumbStyleFlag
    property bool thumbStyleFlag: hoverTransformationFlag || draggingThumb
    //only show the thumb if the page AND the view is moving,
    //don't keep it on screen just because the page is long
                                  || (veryLongContentItem && (flickableItem.moving || scrollAnimation.running))

    property bool draggingThumb: thumbArea.drag.active
    property PropertyAnimation scrollbarThicknessAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    //duration coming from the UX spec
    property PropertyAnimation scrollbarFadeInAnimation: UbuntuNumberAnimation { duration: 500/*UbuntuAnimation.SnapDuration*/}
    property PropertyAnimation scrollbarFadeOutAnimation: UbuntuNumberAnimation { duration: 500/*UbuntuAnimation.SlowDuration*/ }
    property int scrollbarFadeOutPause: 3000
    //the time before a thumb style scrollbar goes back to indicator style after the mouse has left
    //the proximity area
    property int scrollbarCollapsePause: 1000
    property PropertyAnimation sliderAnimation: UbuntuNumberAnimation {}
    //property PropertyAnimation thumbConnectorFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    //property PropertyAnimation thumbFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }

    property color troughColorThumbStyle: "#CDCDCD"
    property color troughColorSteppersStyle: "#f7f7f7"
    property color sliderColor: "#5d5d5d"
    property string sliderRadius: units.dp(2)

    property real marginFromEdge: units.gu(1)

    // helper properties to ease code readability
    property Flickable flickableItem: styledItem.flickableItem
    property bool isScrollable: styledItem.__private.scrollable && pageSize > 0.0
                                && contentSize > 0.0 && contentSize > pageSize
    property bool isVertical: ScrollbarUtils.isVertical(styledItem)
    property bool frontAligned: (styledItem.align === Qt.AlignLeading)
    property bool rearAligned: (styledItem.align === Qt.AlignTrailing)
    property bool topAligned: (styledItem.align === Qt.AlignTop)
    property bool bottomAligned: (styledItem.align === Qt.AlignBottom)

    property real pageSize: (isVertical) ? (styledItem.flickableItem.height) : (styledItem.flickableItem.width)
    property real contentSize: (isVertical) ? styledItem.flickableItem.contentHeight : styledItem.flickableItem.contentWidth
    property real leadingContentMargin: isVertical ? styledItem.flickableItem.topMargin : styledItem.flickableItem.leftMargin
    property real trailingContentMargin: isVertical ? styledItem.flickableItem.bottomMargin : styledItem.flickableItem.rightMargin

    //this size includes content margins
    property real totalContentSize: contentSize + leadingContentMargin + trailingContentMargin

    property Item trough: trough
    property alias __overshootTimer: overshootTimer

    //used by scrollbarUtils to query its size (trying to avoid changing the scrollbarUtils API)
    property alias thumb: slider

    //internals
    property bool __recursionGuard: false
    property bool __disableStateBinding: false

    function scroll(amount) {
        slider.scroll(amount)
    }
    function scrollToBeginning() {
        scrollAnimation.to = 0
        scrollAnimation.restart()
    }
    function scrollToEnd() {
        scrollAnimation.to = contentSize - pageSize
        scrollAnimation.restart()
    }
    function resetScrollingToPreDrag() {
        thumbArea.resetFlickableToPreDragState()
    }

    /*****************************************
      Visuals
     *****************************************/
    anchors.fill: parent
    opacity: overlayOpacityWhenHidden

    Binding {
        when: !__disableStateBinding
        target: visuals
        property: 'state'
        value: {
            console.log("REEVALUATING STATE\n",
                        "isScrollable", isScrollable,
                        "alwaysOnScrollbars", alwaysOnScrollbars,
                        "overlay", overlay, "\n",
                        "hoverTransformationFlag", hoverTransformationFlag,
                        "draggingThumb", draggingThumb,
                        "overshoot timer running", __overshootTimer.running, "\n",
                        "thumbStyleFlag", thumbStyleFlag)
            if (!isScrollable && !alwaysOnScrollbars)
                return '';
            else if (overlay) {
                //we use the check on running to avoid to make it so that the scrollbar completes the transition
                //to the steppers (or thumb) state even when the mouse exits the area prematurely (e.g. when the mouse
                //overshoots, without the .running check the scrollbar would stop the growing transition halfway and
                //go back to hidden mode)
                if (hoverTransformationFlag || draggingThumb || __overshootTimer.running) {
                    return 'steppers'
                } else if (thumbStyleFlag || __overshootTimer.running) {
                    return 'thumb'
                } else if (flickableItem.moving || scrollAnimation.running || transitionToIndicator.running) {
                    return 'indicator';
                } else return 'hidden';
            } else {
                return 'steppers'
                //                if (proximityArea.containsMouse || growingTransition.running/*|| (styledItem.buddyScrollbar //NEW UX DIRECTIONS: only one scrollbar at a time can be in thumb style
                //                            && styledItem.buddyScrollbar.__styleInstance
                //                            && styledItem.buddyScrollbar.__styleInstance.thumbStyleFlag)*/) {
                //                    return 'steppers';
                //                } else
                //                    return 'indicator';
            }
        }
    }

    onStateChanged: console.log("STATE", state, visuals)

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
        },
        State {
            name: 'indicator'
            extend: 'shown'
            PropertyChanges {
                target: flowContainer
                thickness: troughThicknessIndicatorStyle
                thumbThickness: indicatorThickness
                showSteppers: false
                showTrough: alwaysOnScrollbars
                showCornerRect: false
            }
            PropertyChanges {
                target: steppersMouseArea
                width: 0
                height: 0
            }
            PropertyChanges {
                target: trough
                color: Qt.rgba(troughColorThumbStyle.r,
                               troughColorThumbStyle.g,
                               troughColorThumbStyle.b,
                               troughColorThumbStyle.a * 0.2/*(overlay ? 0.3 : 1.0)*/)
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
                showTrough: alwaysOnScrollbars
                showCornerRect: false
            }
            PropertyChanges {
                target: steppersMouseArea
                width: 0
                height: 0
            }
            PropertyChanges {
                target: trough
                color: Qt.rgba(troughColorThumbStyle.r,
                               troughColorThumbStyle.g,
                               troughColorThumbStyle.b,
                               troughColorThumbStyle.a * 0.2/*(overlay ? 0.3 : 1.0)*/)
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
                showTrough: true
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
                target: trough
                color: Qt.rgba(troughColorSteppersStyle.r,
                               troughColorSteppersStyle.g,
                               troughColorSteppersStyle.b,
                               troughColorSteppersStyle.a)
            }
        }
    ]
    transitions: [
        Transition {
            from: ''
            to: 'shown'
            onRunningChanged: console.log("RUNNING SHOWN TRANSITION", running)
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
            //we use __disableStateBinding as the proof that the indicator style we are in was just
            //for brief hinting purposes (in that case, in fact, the binding to the "state" var is disabled)
            onRunningChanged: {
                console.log("COLLAPSING TO INDICATOR transition running changed", running, __disableStateBinding, hintingStyle, visuals)

                if (!running && __disableStateBinding && hintingStyle === 'indicator') {
                    //this handles the case when we're briefly showing the scrollbar
                    //as a hint, as a consequence of the resizing of the flickable item or
                    //its content item
                    console.log("END FLASHING INDICATOR STYLE")
                    __disableStateBinding = false
                }
            }
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
                    ColorAnimation { target: trough; duration: scrollbarThicknessAnimation.duration }
                }
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showTrough,showCornerRect"
                }
            }
        },
        Transition {
            id: transitionToIndicator
            from: '*'
            to: 'indicator'
            //we use __disableStateBinding as the proof that the indicator style we are in was just
            //for brief hinting purposes (in that case, in fact, the binding to the "state" var is disabled)
            onRunningChanged: {
                console.log("INDICATOR transition running changed", running, __disableStateBinding, hintingStyle, visuals)

                if (!running && __disableStateBinding && hintingStyle === 'indicator') {
                    //this handles the case when we're briefly showing the scrollbar
                    //as a hint, as a consequence of the resizing of the flickable item or
                    //its content item
                    console.log("END FLASHING INDICATOR STYLE")
                    __disableStateBinding = false
                }
            }

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
                    ColorAnimation { target: trough; duration: scrollbarThicknessAnimation.duration }
                }
                //this covers the case when scrollbar is still transitioning from steppers style to hidden,
                //but the transition is not yet finished, and we flick the view. At that point the state is "hidden"
                //but the properties are still transitioning from steppers style to hidden. If a change to "indicator"
                //style is triggered while the properties are still transitioning, we have to handle those properties
                //as well to avoid immediate changes in the UI.
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showTrough,showCornerRect"
                }
            }
        },
        Transition {
            id: growingTransition
            from: 'indicator'
            to: 'thumb,steppers'
            reversible: true
            SequentialAnimation {
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showTrough,showCornerRect"
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
                    ColorAnimation { target: trough; duration: scrollbarThicknessAnimation.duration }
                }
            }
        },
        Transition {
            id: steppersTransition
            from: '*'
            to: 'thumb,steppers'
            onRunningChanged:
            {
                console.log("THUMB transition running changed", running, __disableStateBinding, hintingStyle, visuals)
                if (!running && __disableStateBinding && hintingStyle === 'thumb') {
                    //this handles the case when we're briefly showing the scrollbar
                    //as a hint, as a consequence of the resizing of the flickable item or
                    //its content item
                    console.log("END FLASHING THUMB STYLE")
                    __disableStateBinding = false
                }
            }
            SequentialAnimation {
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers,showTrough,showCornerRect"
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
                    ColorAnimation { target: trough; duration: visuals.opacity > 0 ? scrollbarThicknessAnimation.duration : 0 }
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
                    properties: "thickness,thumbThickness,showSteppers,showTrough,showCornerRect"
                }
                PropertyAction {
                    target: trough
                    properties: "color"
                }
                PropertyAction {
                    target: visuals
                    properties: "state"
                }
            }
        }
    ]

    function mapToPoint(map)
    {
        return Qt.point(map.x, map.y)
    }

    //As per spec, we show a hint of the scrollbars in indicator style whenever the
    //flickable item changes its size
    //THIS IS ASSUMING THAT the transition to hidden state has a PauseAnimation, otherwise
    //the hinting would be too quick!
    function flashScrollbar() {
        //only show the hint if the scrollbar is currently hidden or its already showing the hint
        //(it could happen that while it is showing the hint the size of the flickable changes enough
        //to trigger the transition to thumb style, and in that case we want to hint again using thumb
        //style)
        if (initialized && isScrollable && (state == '' || state === 'hidden' || (__disableStateBinding && state !== hintingStyle))) {
            console.log("STARTING HINT", __disableStateBinding, state, hintingStyle, visuals)
            __disableStateBinding = true
            state = hintingStyle
        }
    }
    //each scrollbar connects to both width and height because we want to show both the scrollbar in
    //both cases
    Connections {
        target: (flickableItem && initialized) ? flickableItem : null
        onContentHeightChanged: flashScrollbar()
        onHeightChanged: flashScrollbar()
        onContentWidthChanged: flashScrollbar()
        onWidthChanged: flashScrollbar()
    }

    SmoothedAnimation {
        id: scrollAnimation
        //duration and easing coming from UX spec
        duration: 500
        easing.type: Easing.InOutCubic
        target: styledItem.flickableItem
        property: (isVertical) ? "contentY" : "contentX"
    }

    Flow {
        id: flowContainer
        anchors {
            fill: parent
            leftMargin: (!isVertical || frontAligned) ? 0.0 : flowContainer.proximityThickness
            rightMargin: (!isVertical || rearAligned) ? 0.0 : flowContainer.proximityThickness
            topMargin: (isVertical || topAligned) ? 0.0 : flowContainer.proximityThickness
            bottomMargin: (isVertical || bottomAligned) ? 0.0 : flowContainer.proximityThickness
        }

        clip: true

        property real thickness: 0
        property real thumbThickness: 0

        property real proximityThickness: (isVertical)
                                          ? styledItem.width - thickness
                                          : styledItem.height - thickness

        property bool showSteppers: false
        property bool showTrough: false
        property bool showCornerRect: false

        flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight

        Item {
            id: troughContainer
            //account for 2 steppers at the end of the scrollbar
            width: isVertical ? parent.width
                              : parent.width - steppersMouseArea.width
            //- (cornerRect.visible ? cornerRect.width : 0))
            height: isVertical ? parent.height - steppersMouseArea.height
                                 //- (cornerRect.visible ? cornerRect.height : 0))
                               : parent.height

            // represents the visible area of the scrollbar where slider and thumb connector are placed
            Rectangle {
                id: trough
                anchors.fill: parent
                //transition.running is needed because we don't want the color to change while
                //the transition to another style is still ongoing
                //radius: (visuals.state === 'steppers' || collapsingTransition.running) ? 0 : sliderRadius
                visible: flowContainer.showTrough
            }

            // The presence of a mouse enables the interactive thumb
            // FIXME: Should use form factor hints
            InverseMouse.onEntered: {
                console.log("FIXME: INVERSE AREA ENTERED, setting interactive to true. THIS IS NOT RELIABLE!")
                interactive = true
            }

            // The slider's position represents which part of the flickable is visible.
            // The slider's size represents the size the visible part relative to the
            // total size of the flickable.
            Item {

//                Rectangle {
//                    anchors.fill: parent
//                    color: "green"
//                }

                id: scrollCursor
                //x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.width - scrollCursor.width - thumbsExtremesMargin)
                //y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.height - scrollCursor.height - thumbsExtremesMargin)
                //width: (isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, 0.0, trough.width - thumbsExtremesMargin*2)
                //height: (!isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, 0.0, trough.height - thumbsExtremesMargin*2)

                function drag() {
                    ScrollbarUtils.dragAndClamp(styledItem, slider.relThumbPosition, totalContentSize, pageSize);
                }
            }

            Rectangle {
                id: slider
                color: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b, sliderColor.a *
                               (thumbArea.drag.active || (thumbArea.pressed && slider.containsMouse && !pressHoldTimer.running) ? 1.0 : 0.7))
                anchors {
                    verticalCenter: (isVertical) ? undefined : trough.verticalCenter
                    horizontalCenter: (isVertical) ? trough.horizontalCenter : undefined
                }

                property bool containsMouse: contains(Qt.point(slider.mapFromItem(thumbArea, thumbArea.mouseX, thumbArea.mouseY).x,
                                                               slider.mapFromItem(thumbArea, thumbArea.mouseX, thumbArea.mouseY).y))

                //the proportional position of the thumb relative to the trough it can move into
                //It is 0 when the thumb is at its minimum value, and 1 when it is at its maximum value
                property real relThumbPosition : isVertical
                                                 ? (slider.y - thumbsExtremesMargin) / (trough.height - 2*thumbsExtremesMargin - slider.height)
                                                 : (slider.x - thumbsExtremesMargin) / (trough.width - 2*thumbsExtremesMargin - slider.width)

                x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.width - slider.width - thumbsExtremesMargin)
                y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, thumbsExtremesMargin, trough.height - slider.height - thumbsExtremesMargin)
                width: (isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, units.gu(5), trough.width - 2 * thumbsExtremesMargin)
                height: (!isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, flowContainer.thumbThickness, trough.height - 2 * thumbsExtremesMargin)
                radius: visuals.sliderRadius

                function scroll(amount) {
                    scrollAnimation.to = ScrollbarUtils.scrollAndClamp(styledItem, amount, 0.0,
                                                                       Math.max(totalContentSize - pageSize, 0));
                    console.log(scrollAnimation.to)
                    scrollAnimation.restart();
                }
            }

            MouseArea {
                id: thumbArea

                //Rectangle {
                //    anchors.fill: parent
                //    color: "pink"
                //}

                anchors {
                    fill: trough
                    // set margins adding 2 dp for error area
                    leftMargin: (!isVertical || frontAligned) ? 0 : units.dp(-2)
                    rightMargin: (!isVertical || rearAligned) ? 0 : units.dp(-2)
                    topMargin: (isVertical || topAligned) ?  0 : units.dp(-2)
                    bottomMargin: (isVertical || bottomAligned) ?  0 : units.dp(-2)
                }
                enabled: isScrollable && interactive //&& alwaysOnScrollbars
                onPressed: {
                    handlePress(mouse.x, mouse.y)
                    //don't start the press and hold timer to avoid conflicts with the drag
                    var mappedCoord = mapToItem(slider, mouseX, mouseY)
                    if (!slider.contains(Qt.point(mappedCoord.x, mappedCoord.y))) {
                        pressHoldTimer.startTimer(thumbArea)
                    }
                }

                function handlePress(mouseX, mouseY) {
                    if (!thumbArea.pressed) return

                    //TODO: not worth initializing this at every press, move outside!
                    var scrollingProp = isVertical ? "y" : "x"
                    var sizeProp = isVertical ? "height" : "width"
                    var mouseScrollingProp = isVertical ? mouseY : mouseX
                    if (mouseScrollingProp < slider[scrollingProp]) {
                        console.log("SCROLLING UP")
                        slider.scroll(-pageSize*visuals.longScrollingRatio)
                    } else if (mouseScrollingProp > slider[scrollingProp] + slider[sizeProp]) {
                        console.log("SCROLL DOWN")
                        slider.scroll(pageSize*visuals.longScrollingRatio)
                    }
                }

                function saveFlickableScrollingState() {
                    originXAtDragStart = flickableItem.originX
                    originYAtDragStart = flickableItem.originY
                    contentXAtDragStart = flickableItem.contentX
                    contentYAtDragStart = flickableItem.contentY
                }
                function resetFlickableToPreDragState() {
                    flickableItem.contentX = originXAtDragStart + contentXAtDragStart
                    flickableItem.contentY = originYAtDragStart + contentYAtDragStart
                }

                property int originXAtDragStart
                property int originYAtDragStart
                property int contentXAtDragStart
                property int contentYAtDragStart

                drag {
                    //don't start a drag while we're scrolling using press and hold
                    target: pressHoldTimer.running ? undefined : slider
                    axis: (isVertical) ? Drag.YAxis : Drag.XAxis
                    minimumY: thumbsExtremesMargin
                    maximumY: trough.height - slider.height - thumbsExtremesMargin
                    minimumX: thumbsExtremesMargin
                    maximumX: trough.width - slider.width - thumbsExtremesMargin
                    onMaximumXChanged: console.log("MAX X!", drag.maximumX)
                    onActiveChanged: {
                        if (drag.active) {
                            thumbArea.saveFlickableScrollingState()
                            scrollCursor.drag()
                        }
                    }

                    //NOTE: we need threshold to be 0, otherwise it will be impossible to drag
                    //contentItems which have a size so that "flickableItem.height - contentItem.height < threshold"!!!
                    threshold: 0
                }

                // drag slider and content to the proper position
                onPositionChanged: {
                    if (pressedButtons == Qt.LeftButton && drag.active) {
                        console.log("DRAGGING")
                        if ((isVertical && Math.abs(mouse.x - thumbArea.x) >= flowContainer.thickness * 10)
                                || (!isVertical && Math.abs(mouse.y - thumbArea.y) >= flowContainer.thickness * 10)) {
                            resetFlickableToPreDragState()
                        } else {
                            scrollCursor.drag()
                        }
                    }
                }

                //onExited: pressHoldTimer.stop()
                onCanceled: pressHoldTimer.stop()
                onReleased: pressHoldTimer.stop()

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
                            console.log("BUG! press and hold timer running without knowing the item that started it. Please report this.")
                        }
                    }
                    repeat: true
                    onRunningChanged: {
                        if (!running) {
                            console.log("STOP TIMER")
                            firstTrigger = true
                        }
                    }
                }

            }
        }
        MouseArea {
            id: steppersMouseArea

            //size is handled by the states

            enabled: isScrollable && interactive
            visible: flowContainer.showSteppers

            function handlePress() {
                var mappedCoordFirstStepper = mapToItem(firstStepper, mouseX, mouseY)
                var mappedCoordSecondStepper = mapToItem(secondStepper, mouseX, mouseY)

                if (firstStepper.contains(Qt.point(mappedCoordFirstStepper.x, mappedCoordFirstStepper.y))) {
                    slider.scroll(-pageSize * visuals.shortScrollingRatio)
                } else if (secondStepper.contains(Qt.point(mappedCoordSecondStepper.x, mappedCoordSecondStepper.y))) {
                    slider.scroll(pageSize * visuals.shortScrollingRatio)
                }
            }

            onPressed: {
                handlePress()
                pressHoldTimer.startTimer(steppersMouseArea)
            }
            onReleased: pressHoldTimer.stop()
            onCanceled: pressHoldTimer.stop()

            Rectangle  {
                id: firstStepper

                color: trough.color
                visible: parent.visible
                clip: true

                anchors {
                    left: parent.left //it's left in both cases, otherwise using RTL would break the layout of the arrow
                    right: isVertical ? parent.right : undefined
                    top: parent.top
                    bottom: !isVertical ? parent.bottom : undefined
                }
                Binding {
                    target: firstStepper
                    property: "height"
                    when: isVertical
                    value: visible ? units.gu(2) : 0
                }
                Binding {
                    target: firstStepper
                    property: "width"
                    when: !isVertical
                    value: visible ? units.gu(2) : 0
                }

                Image {
                    anchors.centerIn: parent
                    //fillMode: Image.PreserveAspectFit
                    rotation: isVertical ? 180 : 90
                    source: Qt.resolvedUrl("../artwork/scrollbar_arrow.png")
                    opacity: 0.5
                }
            }
            Rectangle {
                id: secondStepper
                color: trough.color
                clip: true

                visible: parent.visible

                anchors {
                    left: isVertical ? parent.left : firstStepper.right
                    right: isVertical ? parent.right : undefined
                    top: !isVertical ? parent.top : firstStepper.bottom
                    bottom: !isVertical ? parent.bottom : undefined
                }

                Binding {
                    target: secondStepper
                    property: "height"
                    when: isVertical
                    value: visible ? units.gu(2) : 0
                }
                Binding {
                    target: secondStepper
                    property: "width"
                    when: !isVertical
                    value: visible ? units.gu(2) : 0
                }
                Image {
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit

                    rotation: isVertical ? 0 : -90
                    source: Qt.resolvedUrl("../artwork/scrollbar_arrow.png") //Qt.resolvedUrl("../artwork/scrollbar_arrow.png")
                    opacity: 0.5
                }
            }
        }
    }

    //just a hack: a rectangle which covers the corner where scrollbars meet, when they're in steppers style
    Rectangle {
        id: cornerRect
        anchors.left: {
            if (styledItem.buddyScrollbar && styledItem.buddyScrollbar.__styleInstance
                    && styledItem.buddyScrollbar.__styleInstance.isScrollable) {
                if (isVertical) {
                    return flowContainer.left
                } else {
                    if (styledItem.buddyScrollbar.align === Qt.AlignTrailing) {
                        return flowContainer.right
                    } else
                        return undefined
                }
            }
        }
        anchors.right: {
            if (styledItem.buddyScrollbar && styledItem.buddyScrollbar.__styleInstance
                    && styledItem.buddyScrollbar.__styleInstance.isScrollable) {
                if (isVertical) {
                    return flowContainer.right
                } else {
                    if (styledItem.buddyScrollbar.align === Qt.AlignLeading) {
                        return flowContainer.left
                    } else {
                        return undefined
                    }
                }
            }
        }
        anchors.top: {
            if (styledItem.buddyScrollbar && styledItem.buddyScrollbar.__styleInstance
                    && styledItem.buddyScrollbar.__styleInstance.isScrollable) {
                if (isVertical) {
                    if (styledItem.buddyScrollbar.align === Qt.AlignBottom) {
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
            if (styledItem.buddyScrollbar && styledItem.buddyScrollbar.__styleInstance
                    && styledItem.buddyScrollbar.__styleInstance.isScrollable) {
                if (isVertical) {
                    if (styledItem.buddyScrollbar.align === Qt.AlignTop) {
                        return flowContainer.top
                    } else {
                        return undefined
                    }
                } else {
                    return flowContainer.bottom
                }
            }
        }

        //remember the thickness animates during state changes,
        //so we want to bind the right side to the thickness of the scrollbar
        width: isVertical ? flowContainer.thickness : units.gu(2)
        height: isVertical ? units.gu(2) : flowContainer.thickness
        color: trough.color
        visible: flowContainer.showCornerRect && styledItem.buddyScrollbar && styledItem.buddyScrollbar.__styleInstance.isScrollable
    }

    // The thumb appears whenever the mouse gets close enough to the scrollbar
    // and disappears after being for a long enough time far enough of it
    MouseArea {
        id: proximityArea

        //        Rectangle {
        //            anchors.fill: parent
        //            color:"red"
        //            opacity:0.2
        //            visible: parent.enabled
        //        }

        anchors.fill: parent
        propagateComposedEvents: true
        enabled: isScrollable && interactive //&& alwaysOnScrollbars
        hoverEnabled: isScrollable && interactive //&& alwaysOnScrollbars
        onEntered: {
            console.log("PROXIMITY ENTERED")
        }
        onExited: /*if (state === 'steppers')*/ overshootTimer.restart()

        Timer {
            id: overshootTimer
            interval: scrollbarThicknessAnimation.duration * 10
        }

        onPositionChanged: mouse.accepted = false
        onPressed: mouse.accepted = false
        onClicked: mouse.accepted = false
        onReleased: mouse.accepted = false
    }
}
