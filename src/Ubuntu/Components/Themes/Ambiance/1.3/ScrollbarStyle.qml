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
  the styledItem's __inactive property, however should be detected upon runtime based on
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
        - thumbFading: PropertyAnimation - animation for the thumb fading
    * other styling properties
        - color sliderColor: color for the slider
        - color thumbConnectorColor: thumb connector color
        - url forwardThumbReleased: forward thumb image when released
        - url forwardThumbPressed: forward thumb image when pressed
        - url backwardThumbReleased: backward thumb image when released
        - url backwardThumbPressed: backward thumb image when pressed
        //TODO: DELETE OLD STUFF!
        - real scrollAreaThickness: scrollbar area thickness, the area where the
                                    slider, thumb and thumb-connector appear
        - real thumbConnectorMargin: margin of the thumb connector aligned to the
                                    thumb visuals
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
    property bool thumbStyleFlag: proximityArea.containsMouse || thumbArea.drag.active

    property PropertyAnimation scrollbarThicknessAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation scrollbarFadeInAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation scrollbarFadeOutAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SlowDuration }
    property int scrollbarFadeOutPause: 300
    property PropertyAnimation sliderAnimation: UbuntuNumberAnimation {}
    //property PropertyAnimation thumbConnectorFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    property PropertyAnimation thumbFading: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }

    property color newOverlayColor: "#5d5d5d"
    property color troughColorThumbStyle: "#CDCDCD"
    property color troughColorSteppersStyle: "#f7f7f7"
    property color sliderColor: newOverlayColor
    property string sliderRadius: units.dp(2)
    property color thumbConnectorColor: "white"
    property url forwardThumbReleased: (styledItem.align === Qt.AlignLeading || styledItem.align === Qt.AlignTrailing) ? Qt.resolvedUrl("../artwork/ScrollbarBottomIdle.png") : Qt.resolvedUrl("../artwork/ScrollbarRightIdle.png")
    property url forwardThumbPressed: (styledItem.align === Qt.AlignLeading || styledItem.align === Qt.AlignTrailing) ? Qt.resolvedUrl("../artwork/ScrollbarBottomPressed.png") : Qt.resolvedUrl("../artwork/ScrollbarRightPressed.png")
    property url backwardThumbReleased: (styledItem.align === Qt.AlignLeading || styledItem.align === Qt.AlignTrailing) ? Qt.resolvedUrl("../artwork/ScrollbarTopIdle.png") : Qt.resolvedUrl("../artwork/ScrollbarLeftIdle.png")
    property url backwardThumbPressed: (styledItem.align === Qt.AlignLeading || styledItem.align === Qt.AlignTrailing) ? Qt.resolvedUrl("../artwork/ScrollbarTopPressed.png") : Qt.resolvedUrl("../artwork/ScrollbarLeftPressed.png")

    //property real thumbConnectorMargin: units.dp(3)
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

    property bool __recursionGuard: false

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

    /*****************************************
      Visuals
     *****************************************/
    anchors.fill: parent

    opacity: overlayOpacityWhenHidden
    state: {
        if (!isScrollable)
            return '';
        else if (overlay) {
            if (thumbStyleFlag || (styledItem.buddyScrollbar
                        && styledItem.buddyScrollbar.__styleInstance
                        && styledItem.buddyScrollbar.__styleInstance.thumbStyleFlag)) {
                return 'trough';
            } else if (flickableItem.moving) {
                return 'indicator';
            } else
                return 'hidden';
        } else {
            return 'steppers';
        }
    }
    onStateChanged: console.log("HELLO STATE", state === "hidden")

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
            name: 'trough'
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
            from: '*'
            to: 'indicator'
            NumberAnimation {
                target: flowContainer
                property: "thickness"
                duration: scrollbarThicknessAnimation.duration
                easing: scrollbarThicknessAnimation.easing
            }
        },
        Transition {
            from: '*'
            to: 'trough'
            NumberAnimation {
                target: flowContainer
                property: "thickness"
                duration: scrollbarThicknessAnimation.duration
                easing: scrollbarThicknessAnimation.easing
            }
        },
        Transition {
            from: '*'
            to: 'hidden'
            SequentialAnimation {
                PauseAnimation { duration: scrollbarFadeOutPause }
                NumberAnimation {
                    target: visuals
                    property: "opacity"
                    duration: scrollbarFadeOutAnimation.duration
                    easing: scrollbarFadeOutAnimation.easing
                }
                PropertyAction {
                    target: flowContainer
                    properties: "thickness,thumbThickness"
                }
            }
        }
    ]

    function mapToPoint(map)
    {
        return Qt.point(map.x, map.y)
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
                color: alwaysOnScrollbars ? troughColorSteppersStyle
                                          : Qt.rgba(troughColorThumbStyle.r,
                                                    troughColorThumbStyle.g,
                                                    troughColorThumbStyle.b,
                                                    troughColorThumbStyle.a * 0.3)
                radius: alwaysOnScrollbars ? 0 : sliderRadius
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

                color: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b, sliderColor.a * 0.8)
                anchors {
                    verticalCenter: (isVertical) ? undefined : trough.verticalCenter
                    horizontalCenter: (isVertical) ? trough.horizontalCenter : undefined
                }

                x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.width - slider.width)
                y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(styledItem, 0.0, styledItem.height - slider.height)
                width: (isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, flowContainer.thumbThickness, trough.width)
                height: (!isVertical) ? flowContainer.thumbThickness : ScrollbarUtils.sliderSize(styledItem, flowContainer.thumbThickness, trough.height)
                radius: visuals.sliderRadius

                /*Behavior on width {
                    enabled: (!isVertical)
                    NumberAnimation {
                        duration: visuals.sliderAnimation.duration
                        easing: visuals.sliderAnimation.easing
                    }
                }
                Behavior on height {
                    enabled: (isVertical)
                    NumberAnimation {
                        duration: visuals.sliderAnimation.duration
                        easing: visuals.sliderAnimation.easing
                    }
                }*/

                function scroll(amount) {
                    scrollAnimation.to = ScrollbarUtils.scrollAndClamp(styledItem, amount, 0.0,
                                                                       totalContentSize - pageSize);
                    scrollAnimation.restart();
                }
            }

            // The sliderThumbConnector ensures a visual connection between the slider and the thumb
            //    Rectangle {
            //        id: sliderThumbConnector

            //        property real thumbConnectorMargin: visuals.thumbConnectorMargin
            //        property bool isThumbAboveSlider: (isVertical) ? thumb.y < slider.y : thumb.x < slider.x
            //        anchors {
            //            left: (isVertical) ? trough.left : (isThumbAboveSlider ? thumb.left : slider.right)
            //            right: (isVertical) ? trough.right : (isThumbAboveSlider ? slider.left : thumb.right)
            //            top: (!isVertical) ? trough.top : (isThumbAboveSlider ? thumb.top : slider.bottom)
            //            bottom: (!isVertical) ? trough.bottom : (isThumbAboveSlider ? slider.top : thumb.bottom)

            //            leftMargin : (isVertical) ? 0 : (isThumbAboveSlider ? thumbConnectorMargin : 0)
            //            rightMargin : (isVertical) ? 0 : (isThumbAboveSlider ? 0 : thumbConnectorMargin)
            //            topMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? thumbConnectorMargin : 0)
            //            bottomMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? 0 : thumbConnectorMargin)
            //        }
            //        color: visuals.thumbConnectorColor
            //        opacity: thumb.shown ? 1.0 : 0.0
            //        Behavior on opacity {
            //            NumberAnimation {
            //                duration: visuals.thumbConnectorFading.duration
            //                easing: visuals.thumbConnectorFading.easing
            //            }
            //        }
            //    }

            MouseArea {
                id: thumbArea

                //        property point thumbPoint: mapToPoint(thumb.mapFromItem(thumbArea, mouseX, mouseY))
                //        property point thumbTopPoint: mapToPoint(thumbTop.mapFromItem(thumb, thumbPoint.x, thumbPoint.y))
                //        property point thumbBottomPoint: mapToPoint(thumbBottom.mapFromItem(thumb, thumbPoint.x, thumbPoint.y))
                //        property bool inThumbTop: thumbTop.contains(thumbTopPoint)
                //        property bool inThumbBottom: thumbBottom.contains(thumbBottomPoint)

                /*Rectangle {
            anchors.fill: parent
            color: "pink"
        }*/
                anchors {
                    fill: trough
                    // set margins adding 2 dp for error area
                    leftMargin: (!isVertical || frontAligned) ? 0 : units.dp(-2)
                    rightMargin: (!isVertical || rearAligned) ? 0 : units.dp(-2)
                    topMargin: (isVertical || topAligned) ?  0 : units.dp(-2)
                    bottomMargin: (isVertical || bottomAligned) ?  0 : units.dp(-2)
                }
                enabled: isScrollable && interactive
                hoverEnabled: true
                //onEntered: thumb.show()
                //        onPressed: {
                //            if (isVertical) {
                //                if (mouseY < thumb.y) {
                //                    thumb.placeThumbForeUnderMouse(mouse)
                //                } else if (mouseY > (thumb.y + thumb.height)) {
                //                    thumb.placeThumbRearUnderMouse(mouse)
                //                }
                //            } else {
                //                if (mouseX < thumb.x) {
                //                    thumb.placeThumbForeUnderMouse(mouse)
                //                } else if (mouseX > (thumb.x + thumb.width)) {
                //                    thumb.placeThumbRearUnderMouse(mouse)
                //                }
                //            }
                //        }
                //        onClicked: {
                //            if (inThumbBottom)
                //                slider.scroll(pageSize)
                //            else if (inThumbTop)
                //                slider.scroll(-pageSize)
                //        }
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

                // Dragging behaviour
                //        function resetDrag() {
                //            thumbYStart = slider.y + marginFromEdge //thumb.y
                //            thumbXStart = slider.x + marginFromEdge//thumb.x
                //            dragYStart = drag.target.y
                //            dragXStart = drag.target.x
                //        }

                //property int thumbYStart
                //property int dragYStart
                //property int dragYAmount: thumbArea.drag.target.y - thumbArea.dragYStart
                //property int thumbXStart
                //property int dragXStart
                //property int dragXAmount: thumbArea.drag.target.x - thumbArea.dragXStart
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
                            //resetDrag()
                            scrollCursor.drag()
                        }
                    }
                }
                // update thumb position
                //        onDragYAmountChanged: {
                //            if (drag.active) {
                //                thumb.y = MathUtils.clamp(thumbArea.thumbYStart + thumbArea.dragYAmount, 0, thumb.maximumPos);
                //            }
                //        }
                //        onDragXAmountChanged: {
                //            if (drag.active) {
                //                thumb.x = MathUtils.clamp(thumbArea.thumbXStart + thumbArea.dragXAmount, 0, thumb.maximumPos);
                //            }
                //        }

                // drag slider and content to the proper position
                onPositionChanged: {
                    if (pressedButtons == Qt.LeftButton && drag.active) {
                        console.log("DRAGGING")
                        scrollCursor.drag()
                    }
                }

                //onExited: pressHoldTimer.stop()
                onCanceled: pressHoldTimer.stop()
                onReleased: pressHoldTimer.stop()

                Timer {
                    id: pressHoldTimer

                    //This is needed to reuse the same timer to handle
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

            Rectangle {
                anchors.fill: parent
                color: "pink"
            }

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

            Rectangle {
                id: firstStepper
                anchors {
                    left: parent.left //it's left in both cases, otherwise using RTL would break the layout of the arrow
                    right: isVertical ? parent.right : undefined
                    top: parent.top
                    bottom: !isVertical ? parent.bottom : undefined
                }
                //setting them to -1 is okay, as anchors will take care of the size
                //in those cases
                width: isVertical ? -1 : flowContainer.thickness
                height: isVertical ? flowContainer.thickness : -1
                color: "blue"
            }
            Rectangle {
                id: secondStepper
                anchors {
                    left: isVertical ? parent.left : firstStepper.right
                    right: isVertical ? parent.right : undefined
                    top: !isVertical ? parent.top : firstStepper.bottom
                    bottom: !isVertical ? parent.bottom : undefined
                }
                width: isVertical ? -1 : flowContainer.thickness
                height: isVertical ? flowContainer.thickness : -1
                color: "red"
            }
        }
        //just a rectangle which covers the corner where scrollbars meet, when they're in steppers style
        Rectangle {
            id: cornerRect
            width: flowContainer.thickness
            height: width
            color: visuals.troughColorSteppersStyle
            visible: flowContainer.showSteppers
        }
    }


    // The thumb appears whenever the mouse gets close enough to the scrollbar
    // and disappears after being for a long enough time far enough of it
    MouseArea {
        id: proximityArea

        //        Rectangle {
        //            anchors.fill: parent
        //            color:"red"
        //        }

        anchors {
            fill: parent
            //            leftMargin: (!isVertical)  ? 0 : (frontAligned ? flowContainer.thickness : 0)
            //            rightMargin: (!isVertical) ? 0 : (rearAligned ? flowContainer.thickness : 0)
            //            topMargin: (isVertical) ? 0 : (topAligned ? flowContainer.thickness : 0)
            //            bottomMargin: (isVertical) ? 0 : (bottomAligned ? flowContainer.thickness : 0)
        }
        propagateComposedEvents: true
        enabled: isScrollable && interactive
        hoverEnabled: true
        onEntered: {
            console.log("PROXIMITY ENTERED")
            //thumb.show();
        }

        onPressed: mouse.accepted = false
        onClicked: mouse.accepted = false
        onReleased: mouse.accepted = false
    }

    Timer {
        id: autohideTimer

        interval: 1000
        repeat: true
        //onTriggered: if (!proximityArea.containsMouse && !thumbArea.containsMouse && !thumbArea.pressed) thumb.hide()
    }

    //    Item {
    //        id: thumb
    //        objectName: "interactiveScrollbarThumb"

    //        enabled: interactive

    //        anchors {
    //            left: frontAligned ? slider.left : undefined
    //            right: rearAligned ? slider.right : undefined
    //            top: topAligned ? slider.top : undefined
    //            bottom: bottomAligned ? slider.bottom : undefined
    //        }

    //        width: childrenRect.width
    //        height: childrenRect.height

    //        property bool shown
    //        property int maximumPos: (isVertical) ? styledItem.height - thumb.height : styledItem.width - thumb.width

    //        /* Show the thumb as close as possible to the mouse pointer */
    //        onShownChanged: {
    //            if (shown) {
    //                if (isVertical) {
    //                    var mouseY = proximityArea.containsMouse ? proximityArea.mouseY : thumbArea.mouseY;
    //                    y = MathUtils.clamp(mouseY - thumb.height / 2, 0, thumb.maximumPos);
    //                } else {
    //                    var mouseX = proximityArea.containsMouse ? proximityArea.mouseX : thumbArea.mouseX;
    //                    x = MathUtils.clamp(mouseX - thumb.width / 2, 0, thumb.maximumPos);
    //                }
    //            }
    //        }

    //        function show() {
    //            autohideTimer.restart();
    //            shown = true;
    //        }

    //        function hide() {
    //            autohideTimer.stop();
    //            shown = false;
    //        }

    //        function placeThumbForeUnderMouse(mouse) {
    //            var diff = (isVertical) ? mouse.y - height / 4 : mouse.x - width / 4;
    //            positionAnimation.to = MathUtils.clamp(diff, 0, maximumPos);
    //            positionAnimation.restart();
    //        }

    //        function placeThumbRearUnderMouse(mouse) {
    //            var diff = (isVertical) ? mouse.y - height * 3 / 4 : mouse.x - width * 3 / 4;
    //            positionAnimation.to = MathUtils.clamp(diff, 0, maximumPos);
    //            positionAnimation.restart();
    //        }

    //        NumberAnimation {
    //            id: positionAnimation

    //            duration: 100
    //            easing.type: Easing.InOutQuad
    //            target: thumb
    //            property: (isVertical) ? "y" : "x"
    //        }

    //        opacity: shown ? (thumbArea.containsMouse || thumbArea.drag.active ? 1.0 : 0.5) : 0.0
    //        Behavior on opacity {
    //            NumberAnimation {
    //                duration: visuals.thumbFading.duration
    //                easing: visuals.thumbFading.easing
    //            }
    //        }

    //        Flow {
    //            // disable mirroring as thumbs are placed in the same way no matter of RTL or LTR
    //            LayoutMirroring.enabled: false
    //            flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight
    //            Image {
    //                id: thumbTop
    //                source: thumbArea.inThumbTop && thumbArea.pressed ? visuals.backwardThumbPressed : visuals.backwardThumbReleased
    //            }
    //            Image {
    //                id: thumbBottom
    //                source: thumbArea.inThumbBottom && thumbArea.pressed ? visuals.forwardThumbPressed : visuals.forwardThumbReleased
    //            }
    //        }
    //    }
}
