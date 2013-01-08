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
import Ubuntu.Components 0.1

/*
  The visuals handle both active and passive modes. This behavior is driven yet by
  the item's __inactive property, however should be detected upon runtime based on
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
        - real scrollAreaThickness: scrollbar area thickness, the area where the
                                    slider, thumb and thumb-connector appear
        - real thumbConnectorMargin: margin of the thumb connector aligned to the
                                    thumb visuals
  */

Item {
    id: visuals
    // helper properties to ease code readability
    property Flickable flickableItem: item.flickableItem
    property bool isListView: ScrollbarUtils.isListView(flickableItem)
    property bool interactive: item.__interactive
    property bool isScrollable: item.__private.scrollable && pageSize > 0.0
                                && contentSize > 0.0 && contentSize > pageSize
    property bool isVertical: (item.__private.vertical)
    property bool frontAligned: (item.align === Qt.AlignLeading)
    property bool rearAligned: (item.align === Qt.AlignTrailing)
    property bool topAligned: (item.align === Qt.AlignTop)
    property bool bottomAligned: (item.align === Qt.AlignBottom)

    property real pageSize: (isVertical) ? item.height : item.width
    property real contentSize: (isVertical) ? item.flickableItem.contentHeight : item.flickableItem.contentWidth
    property real overlayOpacityWhenShown: StyleUtils.itemStyleProperty("overlayOpacityWhenShown", 0.6)
    property real overlayOpacityWhenHidden: StyleUtils.itemStyleProperty("overlayOpacityWhenHidden", 0.0)
    property bool overlay: StyleUtils.itemStyleProperty("overlay", false) && !interactive

    property real minimumSliderSize: StyleUtils.itemStyleProperty("minimumSliderSize", units.gu(2))

    /*****************************************
      Visuals
     *****************************************/
    anchors.fill: parent

    opacity: overlayOpacityWhenHidden
    state: {
        if (!isScrollable)
            return '';
        else if (overlay) {
            if (flickableItem.moving)
                return 'overlay';
            else
                return 'stopped';
        } else
            return 'shown';
    }

    states: [
        State {
            name: 'stopped'
            extend: ''
            PropertyChanges {
                target: visuals
                opacity: overlayOpacityWhenHidden
            }
        },
        State {
            name: "shown"
            PropertyChanges {
                target: visuals
                opacity: overlayOpacityWhenShown
            }
        },
        State {
            name: 'overlay'
            PropertyChanges {
                target: visuals
                opacity: overlayOpacityWhenShown
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
                duration: StyleUtils.itemStyleProperty("scrollbarFadeInAnimation").duration
                easing: StyleUtils.itemStyleProperty("scrollbarFadeInAnimation").easing
            }
        },
        Transition {
            from: '*'
            to: 'overlay'
            NumberAnimation {
                target: visuals
                property: "opacity"
                duration: StyleUtils.itemStyleProperty("scrollbarFadeInAnimation").duration
                easing: StyleUtils.itemStyleProperty("scrollbarFadeInAnimation").easing
            }
        },
        Transition {
            from: "overlay"
            to: "stopped"
            SequentialAnimation {
                PauseAnimation { duration: StyleUtils.itemStyleProperty("scrollbarFadeOutPause", 0) }
                NumberAnimation {
                    target: visuals
                    property: "opacity"
                    duration: StyleUtils.itemStyleProperty("scrollbarFadeOutAnimation").duration
                    easing: StyleUtils.itemStyleProperty("scrollbarFadeOutAnimation").easing
                }
            }
        }
    ]

    function mapToPoint(map)
    {
        return Qt.point(map.x, map.y)
    }

    SystemPalette { id: systemColors }

    SmoothedAnimation {
        id: scrollAnimation

        duration: 200
        easing.type: Easing.InOutQuad
        target: item.flickableItem
        property: (isVertical) ? "contentY" : "contentX"
    }

    // represents the visible area of the scrollbar where slider and thumb connector are placed
    Item {
        id: scrollbarArea

        property real thickness: StyleUtils.itemStyleProperty("scrollAreaThickness", units.dp(2))
        property real proximityThickness: (isVertical) ? item.width - thickness : item.height - thickness
        anchors {
            fill: parent
            leftMargin: (!isVertical || frontAligned) ? 0 : proximityThickness
            rightMargin: (!isVertical || rearAligned) ? 0 : proximityThickness
            topMargin: (isVertical || topAligned) ? 0 : proximityThickness
            bottomMargin: (isVertical || bottomAligned) ? 0 : proximityThickness
        }
    }
    // The thumb appears whenever the mouse gets close enough to the scrollbar
    // and disappears after being for a long enough time far enough of it
    MouseArea {
        id: proximityArea

        anchors {
            fill: parent
            leftMargin: (!isVertical)  ? 0 : (frontAligned ? scrollbarArea.thickness : 0)
            rightMargin: (!isVertical) ? 0 : (rearAligned ? scrollbarArea.thickness : 0)
            topMargin: (isVertical) ? 0 : (topAligned ? scrollbarArea.thickness : 0)
            bottomMargin: (isVertical) ? 0 : (bottomAligned ? scrollbarArea.thickness : 0)
        }
        propagateComposedEvents: true
        enabled: isScrollable && interactive
        hoverEnabled: true
        onEntered: thumb.show();
        onPressed: mouse.accepted = false
        onClicked: mouse.accepted = false
        onReleased: mouse.accepted = false
    }


    // The slider's position represents which part of the flickable is visible.
    // The slider's size represents the size the visible part relative to the
    // total size of the flickable.
    Item {
        id: scrollCursor
        x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(isVertical, flickableItem, item.width, 0.0, item.width - scrollCursor.width)
        y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(isVertical, flickableItem, item.height, 0.0, item.height - scrollCursor.height)
        width: (isVertical) ? scrollbarArea.thickness : ScrollbarUtils.sliderSize(isVertical, flickableItem, 0.0, flickableItem.width)
        height: (!isVertical) ? scrollbarArea.thickness : ScrollbarUtils.sliderSize(isVertical, flickableItem, 0.0, flickableItem.height)

        function drag() {
            ScrollbarUtils.dragAndClamp(scrollCursor, isVertical, flickableItem, contentSize, pageSize);
        }
    }

    Rectangle {
        id: slider

        color: StyleUtils.itemStyleProperty("sliderColor", systemColors.highlight)

        anchors {
            left: (isVertical) ? scrollbarArea.left : undefined
            right: (isVertical) ? scrollbarArea.right : undefined
            top: (!isVertical) ? scrollbarArea.top : undefined
            bottom: (!isVertical) ? scrollbarArea.bottom : undefined
        }

        x: (isVertical) ? 0 : ScrollbarUtils.sliderPos(isVertical, flickableItem, item.width, 0.0, item.width - slider.width)
        y: (!isVertical) ? 0 : ScrollbarUtils.sliderPos(isVertical, flickableItem, item.height, 0.0, item.height - slider.height)
        width: (isVertical) ? scrollbarArea.thickness : ScrollbarUtils.sliderSize(isVertical, flickableItem, minimumSliderSize, flickableItem.width)
        height: (!isVertical) ? scrollbarArea.thickness : ScrollbarUtils.sliderSize(isVertical, flickableItem, minimumSliderSize, flickableItem.height)

        Behavior on width {
            enabled: (!isVertical)
            animation: ScriptAction {
                script: StyleUtils.animate("sliderAnimation")
            }
        }
        Behavior on height {
            enabled: (isVertical)
            animation: ScriptAction {
                script: StyleUtils.animate("sliderAnimation")
            }
        }

        function scroll(amount) {
            scrollAnimation.to = ScrollbarUtils.scrollAndClamp(amount, isVertical, flickableItem, 0.0, contentSize - pageSize);
            scrollAnimation.restart();
        }
    }

    // The sliderThumbConnector ensures a visual connection between the slider and the thumb
    Rectangle {
        id: sliderThumbConnector

        property real thumbConnectorMargin: StyleUtils.itemStyleProperty("thumbConnectorMargin", units.dp(3))
        property bool isThumbAboveSlider: (isVertical) ? thumb.y < slider.y : thumb.x < slider.x
        anchors {
            left: (isVertical) ? scrollbarArea.left : (isThumbAboveSlider ? thumb.left : slider.right)
            right: (isVertical) ? scrollbarArea.right : (isThumbAboveSlider ? slider.left : thumb.right)
            top: (!isVertical) ? scrollbarArea.top : (isThumbAboveSlider ? thumb.top : slider.bottom)
            bottom: (!isVertical) ? scrollbarArea.bottom : (isThumbAboveSlider ? slider.top : thumb.bottom)

            leftMargin : (isVertical) ? 0 : (isThumbAboveSlider ? thumbConnectorMargin : 0)
            rightMargin : (isVertical) ? 0 : (isThumbAboveSlider ? 0 : thumbConnectorMargin)
            topMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? thumbConnectorMargin : 0)
            bottomMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? 0 : thumbConnectorMargin)
        }
        color: StyleUtils.itemStyleProperty("thumbConnectorColor", "white")
        opacity: thumb.shown ? 1.0 : 0.0
        Behavior on opacity {animation: ScriptAction {script: StyleUtils.animate("thumbConnectorFading")}}
    }

    MouseArea {
        id: thumbArea

        property point thumbPoint: mapToPoint(thumb.mapFromItem(thumbArea, mouseX, mouseY))
        property point thumbTopPoint: mapToPoint(thumbTop.mapFromItem(thumb, thumbPoint.x, thumbPoint.y))
        property point thumbBottomPoint: mapToPoint(thumbBottom.mapFromItem(thumb, thumbPoint.x, thumbPoint.y))
        property bool inThumbTop: thumbTop.contains(thumbTopPoint)
        property bool inThumbBottom: thumbBottom.contains(thumbBottomPoint)

        anchors {
            fill: scrollbarArea
            // set margins adding 2 dp for error area
            leftMargin: (!isVertical || frontAligned) ? 0 : units.dp(-2) - thumb.width
            rightMargin: (!isVertical || rearAligned) ? 0 : units.dp(-2) - thumb.width
            topMargin: (isVertical || topAligned) ?  0 : units.dp(-2) - thumb.height
            bottomMargin: (isVertical || bottomAligned) ?  0 : units.dp(-2) - thumb.height
        }
        enabled: isScrollable && interactive
        hoverEnabled: true
        onEntered: thumb.show()
        onPressed: {
            if (isVertical) {
                if (mouseY < thumb.y) {
                    thumb.placeThumbForeUnderMouse(mouse)
                } else if (mouseY > (thumb.y + thumb.height)) {
                    thumb.placeThumbRearUnderMouse(mouse)
                }
            } else {
                if (mouseX < thumb.x) {
                    thumb.placeThumbForeUnderMouse(mouse)
                } else if (mouseX > (thumb.x + thumb.width)) {
                    thumb.placeThumbRearUnderMouse(mouse)
                }
            }
        }
        onClicked: {
            if (inThumbBottom)
                slider.scroll(pageSize)
            else if (inThumbTop)
                slider.scroll(-pageSize)
        }

        // Dragging behaviour
        function resetDrag() {
            thumbYStart = thumb.y
            thumbXStart = thumb.x
            dragYStart = drag.target.y
            dragXStart = drag.target.x
        }

        property int thumbYStart
        property int dragYStart
        property int dragYAmount: thumbArea.drag.target.y - thumbArea.dragYStart
        property int thumbXStart
        property int dragXStart
        property int dragXAmount: thumbArea.drag.target.x - thumbArea.dragXStart
        drag {
            target: scrollCursor
            axis: (isVertical) ? Drag.YAxis : Drag.XAxis
            minimumY: 0
            maximumY: flickableItem.height - scrollCursor.height
            minimumX: 0
            maximumX: flickableItem.width - scrollCursor.width
            onActiveChanged: {
                print(drag.active)
                if (drag.active) resetDrag()
            }
        }
        // update thump position
        onDragYAmountChanged: {
            if (drag.active) {
                thumb.y = MathUtils.clamp(thumbArea.thumbYStart + thumbArea.dragYAmount, 0, thumb.maximumPos);
            }
        }
        onDragXAmountChanged: {
            if (drag.active) {
                thumb.x = MathUtils.clamp(thumbArea.thumbXStart + thumbArea.dragXAmount, 0, thumb.maximumPos);
            }
        }

        // drag slider and content to the proper position
        onPositionChanged: {
            if (pressedButtons == Qt.LeftButton) {
                scrollCursor.drag()
            }
        }
    }

    Timer {
        id: autohideTimer

        interval: 1000
        repeat: true
        onTriggered: if (!proximityArea.containsMouse && !thumbArea.containsMouse && !thumbArea.pressed) thumb.hide()
    }

    Item {
        id: thumb

        enabled: interactive

        anchors {
            left: frontAligned ? slider.left : undefined
            right: rearAligned ? slider.right : undefined
            top: topAligned ? slider.top : undefined
            bottom: bottomAligned ? slider.bottom : undefined
        }

        width: childrenRect.width
        height: childrenRect.height

        property bool shown
        property int maximumPos: (isVertical) ? item.height - thumb.height : item.width - thumb.width

        /* Show the thumb as close as possible to the mouse pointer */
        onShownChanged: {
            if (shown) {
                if (isVertical) {
                    var mouseY = proximityArea.containsMouse ? proximityArea.mouseY : thumbArea.mouseY;
                    y = MathUtils.clamp(mouseY - thumb.height / 2, 0, thumb.maximumPos);
                } else {
                    var mouseX = proximityArea.containsMouse ? proximityArea.mouseX : thumbArea.mouseX;
                    x = MathUtils.clamp(mouseX - thumb.width / 2, 0, thumb.maximumPos);
                }
            }
        }

        function show() {
            autohideTimer.restart();
            shown = true;
        }

        function hide() {
            autohideTimer.stop();
            shown = false;
        }

        function placeThumbForeUnderMouse(mouse) {
            var diff = (isVertical) ? mouse.y - height / 4 : mouse.x - width / 4;
            positionAnimation.to = MathUtils.clamp(diff, 0, maximumPos);
            positionAnimation.restart();
        }

        function placeThumbRearUnderMouse(mouse) {
            var diff = (isVertical) ? mouse.y - height * 3 / 4 : mouse.x - width * 3 / 4;
            positionAnimation.to = MathUtils.clamp(diff, 0, maximumPos);
            positionAnimation.restart();
        }

        NumberAnimation {
            id: positionAnimation

            duration: 100
            easing.type: Easing.InOutQuad
            target: thumb
            property: (isVertical) ? "y" : "x"
        }

        opacity: shown ? (thumbArea.containsMouse || thumbArea.drag.active ? 1.0 : 0.5) : 0.0
        Behavior on opacity {animation: ScriptAction {script: StyleUtils.animate("thumbFading")}}

        property url backwardPressed: StyleUtils.itemStyleProperty("backwardThumbPressed", "")
        property url backwardReleased: StyleUtils.itemStyleProperty("backwardThumbReleased", "")
        property url forwardPressed: StyleUtils.itemStyleProperty("forwardThumbPressed", "")
        property url forwardReleased: StyleUtils.itemStyleProperty("forwardThumbReleased", "")
        Flow {
            // disable mirroring as thumbs are placed in the same way no matter of RTL or LTR
            LayoutMirroring.enabled: false
            flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight
            Image {
                id: thumbTop
                source: thumbArea.inThumbTop && thumbArea.pressed ? thumb.backwardPressed : thumb.backwardReleased
            }
            Image {
                id: thumbBottom
                source: thumbArea.inThumbBottom && thumbArea.pressed ? thumb.forwardPressed : thumb.forwardReleased
            }
        }
    }
}
