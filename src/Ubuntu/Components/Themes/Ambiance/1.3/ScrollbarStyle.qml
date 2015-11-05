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
    property real troughThicknessThumbStyle : units.gu(1)
    property real thumbThickness: units.gu(1)
    property real indicatorThickness : units.dp(4)

    //this is the condition that triggers "Thumb Style"
    //This property is also queried by the "buddyScrollbar" to make sure both scrollbars
    //are in thumb style even when only one of them has a true thumbStyleFlag
    property bool thumbStyleFlag: proximityArea.containsMouse || draggingThumb
                                  || (isVertical ? (flickableItem && flickableItem.contentHeight > flickableItem.height * 10)
                                                 : (flickableItem && flickableItem.contentWidth > flickableItem.width * 10))

    property bool draggingThumb: thumbArea.drag.active
    property PropertyAnimation scrollbarThicknessAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation scrollbarFadeInAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation scrollbarFadeOutAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SlowDuration }
    property int scrollbarFadeOutPause: 300
    //the time before a thumb style scrollbar goes back to indicator style after the mouse has left
    //the proximity area
    property int scrollbarCollapsePause: 1000
    property PropertyAnimation sliderAnimation: UbuntuNumberAnimation {}
    //property PropertyAnimation thumbConnectorFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation thumbFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }

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

    //INTERNAL: simulate the system setting (which will be implemented in unity8, I guess)
    //True --> Steppers style, non-overlay scrollbars
    //False --> Indicator and Trough styles
    property bool alwaysOnScrollbars: styledItem.__alwaysOnScrollbars

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
            console.log("EVALUATING")
            if (!isScrollable && !alwaysOnScrollbars)
                return '';
            else if (overlay) {
                if (thumbStyleFlag || growingTransition.running) {
                    return 'thumb'
                } else if (flickableItem.moving || scrollAnimation.running || transitionToIndicator.running) {
                    return 'indicator';
                } else return 'hidden';
            } else {
                if (proximityArea.containsMouse || growingTransition.running/*|| (styledItem.buddyScrollbar //NEW UX DIRECTIONS: only one scrollbar at a time can be in thumb style
                            && styledItem.buddyScrollbar.__styleInstance
                            && styledItem.buddyScrollbar.__styleInstance.thumbStyleFlag)*/) {
                    return 'steppers';
                } else
                    return 'indicator';
            }
        }
    }

    onStateChanged: console.log("STATE", state)

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
                thickness: indicatorThickness
                thumbThickness: indicatorThickness
                showSteppers: false
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
            }
        },
        State {
            name: 'steppers'
            extend: 'shown'
            PropertyChanges {
                target: flowContainer
                thickness: visuals.troughThicknessSteppersStyle
                thumbThickness: visuals.thumbThickness
                showSteppers: true
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
                PauseAnimation { id: scrollbarCollapseAnim; duration: scrollbarCollapsePause }
                ParallelAnimation {
                    NumberAnimation {
                        target: flowContainer
                        properties: "thickness,thumbThickness"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                    NumberAnimation {
                        target: steppersMouseArea
                        properties: "height,width"
                        duration: scrollbarThicknessAnimation.duration
                        easing: scrollbarThicknessAnimation.easing
                    }
                }
                PropertyAction {
                    target: flowContainer
                    properties: "showSteppers"
                }
            }
        },
        Transition {
            id: transitionToIndicator
            from: '*'
            to: 'indicator'
            //we use __disableStateBinding as the proof that the indicator style we are in was just
            //for brief hinting purposes (in that case, in fact, the binding to the "state" var is disabled)
            onRunningChanged: if (!running && __disableStateBinding) {
                                  //this handles the case when we're briefly showing the scrollbar
                                  //as a hint, as a consequence of the resizing of the flickable item or
                                  //its content item
                                  console.log("END FLASHING INDICATOR STYLE")
                                  __disableStateBinding = false
                              }

            NumberAnimation {
                target: flowContainer
                properties: "thickness,thumbThickness"
                duration: scrollbarThicknessAnimation.duration
                easing: scrollbarThicknessAnimation.easing
            }
        },
        Transition {
            id: growingTransition
            from: '*'
            to: 'thumb,steppers'
            ParallelAnimation {
                NumberAnimation {
                    target: flowContainer
                    properties: "thickness,thumbThickness"
                    duration: scrollbarThicknessAnimation.duration
                    easing: scrollbarThicknessAnimation.easing
                }
                NumberAnimation {
                    target: steppersMouseArea
                    properties: "height,width"
                    duration: scrollbarThicknessAnimation.duration
                    easing: scrollbarThicknessAnimation.easing
                }
            }
        },
        Transition {
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
                    target: flowContainer
                    properties: "thickness,thumbThickness,showSteppers"
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
    function flashIndicator() {
        if (isScrollable && (state == '' || state === 'hidden')) {
            __disableStateBinding = true
            state = 'indicator'
        }
    }
    //each scrollbar connects to both width and height because we want to show both the scrollbar in
    //both cases
    Connections {
        target: flickableItem ? flickableItem : null
        onContentHeightChanged: flashIndicator()
        onHeightChanged: flashIndicator()
        onContentWidthChanged: flashIndicator()
        onWidthChanged: flashIndicator()
    }

    SmoothedAnimation {
        id: scrollAnimation
        duration: 200
        easing.type: Easing.InOutQuad
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
        property real thickness: 0
        property real thumbThickness: 0

        property real proximityThickness: (isVertical)
                                          ? styledItem.width - thickness
                                          : styledItem.height - thickness

        property bool showSteppers: false

        flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight

        Item {
            id: troughContainer
            //account for 2 steppers at the end of the scrollbar
            width: isVertical ? parent.width
                              : (parent.width - steppersMouseArea.width
                                 - (cornerRect.visible ? cornerRect.width : 0))
            height: isVertical ? (parent.height - steppersMouseArea.height
                                  - (cornerRect.visible ? cornerRect.height : 0))
                               : parent.height

            // represents the visible area of the scrollbar where slider and thumb connector are placed
            Rectangle {
                id: trough
                anchors.fill: parent
                //transition.running is needed because we don't want the color to change while
                //the transition to another style is still ongoing
                color: //Qt.rgba(troughColorThumbStyle.r,
                       //        troughColorThumbStyle.g,
                       //        troughColorThumbStyle.b,
                       //        troughColorThumbStyle.a * 0.2)
                       (visuals.state === 'steppers' || collapsingTransition.running) ? Qt.rgba(troughColorSteppersStyle.r,troughColorSteppersStyle.g,troughColorSteppersStyle.b,troughColorSteppersStyle.a * 0.9)
                                                                                      : Qt.rgba(troughColorThumbStyle.r,
                                                                                                troughColorThumbStyle.g,
                                                                                                troughColorThumbStyle.b,
                                                                                                troughColorThumbStyle.a * 0.2/*(overlay ? 0.3 : 1.0)*/)
                radius: (visuals.state === 'steppers' || collapsingTransition.running) ? 0 : sliderRadius
                visible: alwaysOnScrollbars
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

                //Rectangle {
                //    anchors.fill: parent
                //    color: "green"
                //}

                id: scrollCursor
                x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.width - scrollCursor.width)
                y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.height - scrollCursor.height)
                width: (isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, 0.0, trough.width)
                height: (!isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, 0.0, trough.height)

                function drag() {
                    ScrollbarUtils.dragAndClamp(styledItem, scrollCursor, totalContentSize, pageSize);
                }
            }

            Rectangle {
                id: slider
                color: styledItem.focus ? "red" : Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b, sliderColor.a * 0.7/*(overlay ? 0.8 : 1.0)*/)
                anchors {
                    verticalCenter: (isVertical) ? undefined : trough.verticalCenter
                    horizontalCenter: (isVertical) ? trough.horizontalCenter : undefined
                }

                x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.width - slider.width)
                y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.height - slider.height)
                width: (isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, flowContainer.thumbThickness, trough.width)
                height: (!isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, flowContainer.thumbThickness, trough.height)
                radius: visuals.sliderRadius

                function scroll(amount) {
                    scrollAnimation.to = ScrollbarUtils.scrollAndClamp(styledItem, amount, 0.0,
                                                                       totalContentSize - pageSize);
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
                enabled: isScrollable && interactive && alwaysOnScrollbars
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
                        slider.scroll(-pageSize*0.9)
                    } else if (mouseScrollingProp > slider[scrollingProp] + slider[sizeProp]) {
                        console.log("SCROLL DOWN")
                        slider.scroll(pageSize*0.9)
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
                    target: pressHoldTimer.running ? undefined : scrollCursor
                    axis: (isVertical) ? Drag.YAxis : Drag.XAxis
                    minimumY: 0
                    maximumY: trough.height - slider.height
                    minimumX: 0
                    maximumX: trough.width - slider.width
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

                    interval: firstTrigger ? 1000 : 500
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
            width: isVertical ? flowContainer.width
                              : (firstStepper.visible ? firstStepper.width : 0)
                                + (secondStepper.visible ? secondStepper.width : 0)
            height: isVertical ? (firstStepper.visible ? firstStepper.height : 0)
                                 + (secondStepper.visible ? secondStepper.height : 0)
                               : trough.height

            enabled: isScrollable && interactive
            visible: flowContainer.showSteppers

            function handlePress() {
                var mappedCoordFirstStepper = mapToItem(firstStepper, mouseX, mouseY)
                var mappedCoordSecondStepper = mapToItem(secondStepper, mouseX, mouseY)

                if (firstStepper.contains(Qt.point(mappedCoordFirstStepper.x, mappedCoordFirstStepper.y))) {
                    slider.scroll(-pageSize * 0.05)
                } else if (secondStepper.contains(Qt.point(mappedCoordSecondStepper.x, mappedCoordSecondStepper.y))) {
                    slider.scroll(pageSize * 0.05)
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
                    value: units.gu(3)
                }
                Binding {
                    target: firstStepper
                    property: "width"
                    when: !isVertical
                    value: units.gu(3)
                }
                Image {
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                    rotation: isVertical ? 180 : 90
                    source: Qt.resolvedUrl("../artwork/scrollbar_arrow.png")
                }
            }
            Rectangle {
                id: secondStepper
                color: trough.color

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
                    value: units.gu(3)
                }
                Binding {
                    target: secondStepper
                    property: "width"
                    when: !isVertical
                    value: units.gu(3)
                }

                Image {
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit

                    rotation: isVertical ? 0 : -90
                    source: Qt.resolvedUrl("../artwork/scrollbar_arrow.png") //Qt.resolvedUrl("../artwork/scrollbar_arrow.png")
                }
            }
        }
        //just a hack: a rectangle which covers the corner where scrollbars meet, when they're in steppers style
        Rectangle {
            id: cornerRect
            width: flowContainer.thickness
            height: width
            color: visuals.troughColorSteppersStyle
            visible: false //flowContainer.showSteppers
        }
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
        enabled: isScrollable && alwaysOnScrollbars && interactive
        hoverEnabled: isScrollable && alwaysOnScrollbars && interactive
        onEntered: {
            console.log("PROXIMITY ENTERED")
        }

        onPositionChanged: mouse.accepted = false
        onPressed: mouse.accepted = false
        onClicked: mouse.accepted = false
        onReleased: mouse.accepted = false
    }
}
