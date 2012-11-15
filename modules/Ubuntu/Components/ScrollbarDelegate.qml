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
//import Ubuntu.Components 0.1 as Theming

/*
  The visuals handle both active and passive modes. This behavior is driven yet by
  the item's __passive property, however should be detected upon runtime based on
  the device type.
  On active scrollbars, positioning is handled so that the logic updates the flickable's
  X/Y content positions, which is then synched with the contentPosition by the main
  element.
  */

Item {
    id: visuals
    // helper properties to ease code readability
    property bool isActive: item.__interactive
    property bool isScrollable: item.__private.scrollable
    property bool isVertical: (item.__private.vertical)
    property bool frontAligned: (item.align === Qt.AlignLeft)
    property bool rearAligned: (item.align === Qt.AlignRight)
    property bool topAligned: (item.align === Qt.AlignTop)
    property bool bottomAligned: (item.align === Qt.AlignBottom)
    property real contentSize: item.__private.contentSize
    property real pageSize: item.__private.pageSize

    /*****************************************
      Visuals
     *****************************************/
    anchors.fill: parent

    opacity: isScrollable ? 1.0 : 0.0
    Behavior on opacity {animation: itemStyle.fadeAnimation}

    function clamp(x, min, max) {
        return Math.max(min, Math.min(max, x))
    }

    // Linearly project a value x from [xmin, xmax] into [ymin, ymax]
    function projectValue(x, xmin, xmax, ymin, ymax) {
        return ((x - xmin) * ymax - (x - xmax) * ymin) / (xmax - xmin)
    }

    function clampAndProject(x, xmin, xmax, ymin, ymax) {
        return projectValue(clamp(x, xmin, xmax), xmin, xmax, ymin, ymax)
    }

    /* Scroll by amount pixels never overshooting */
    function scrollBy(amount) {
        var destination = item.__private.contentPosition + amount
        destination += (isVertical) ? item.flickableItem.originY : item.flickableItem.originX
        scrollAnimation.to = clamp(destination, 0, contentSize - pageSize)
        scrollAnimation.restart()
    }

    function scrollOnePageBackward() {
        scrollBy(-pageSize)
    }

    function scrollOnePageForward() {
        scrollBy(pageSize)
    }

    function mapToPoint(map)
    {
        return Qt.point(map.x, map.y)
    }

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

        property real thickness: itemStyle.scrollAreaThickness
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
        enabled: isScrollable && isActive
        hoverEnabled: true
        onEntered: thumb.show()
    }


    // The slider's position represents which part of the flickable is visible.
    // The slider's size represents the size the visible part relative to the
    // total size of the flickable.
    Rectangle {
        id: slider

        color: itemStyle.sliderColor

        anchors {
            left: (isVertical) ? scrollbarArea.left : undefined
            right: (isVertical) ? scrollbarArea.right : undefined
            top: (!isVertical) ? scrollbarArea.top : undefined
            bottom: (!isVertical) ? scrollbarArea.bottom : undefined
        }

        // FIXME: theme this
        property int minimalSize: units.gu(4)
        x: (isVertical) ? 0 : clampAndProject(item.__private.contentPosition, 0.0, contentSize - pageSize, 0.0, item.width - slider.width)
        y: (!isVertical) ? 0 : clampAndProject(item.__private.contentPosition, 0.0, contentSize - pageSize, 0.0, item.height - slider.height)
        width: (isVertical) ?
                   scrollbarArea.thickness :
                   clamp(pageSize / contentSize * item.width, minimalSize, item.width)
        height: (!isVertical) ?
                    scrollbarArea.thickness :
                    clamp(pageSize / contentSize * item.height, minimalSize, item.height)

        Behavior on width {
            enabled: (!isVertical)
            animation: itemStyle.sliderAnimation
        }
        Behavior on height {
            enabled: (isVertical)
            animation: itemStyle.sliderAnimation
        }
    }

    // The sliderThumbConnector ensures a visual connection between the slider and the thumb
    Rectangle {
        id: sliderThumbConnector

        property bool isThumbAboveSlider: (isVertical) ? thumb.y < slider.y : thumb.x < slider.x
        anchors {
            left: (isVertical) ? scrollbarArea.left : (isThumbAboveSlider ? thumb.left : slider.right)
            right: (isVertical) ? scrollbarArea.right : (isThumbAboveSlider ? slider.left : thumb.right)
            top: (!isVertical) ? scrollbarArea.top : (isThumbAboveSlider ? thumb.top : slider.bottom)
            bottom: (!isVertical) ? scrollbarArea.bottom : (isThumbAboveSlider ? slider.top : thumb.bottom)

            leftMargin : (isVertical) ? 0 : (isThumbAboveSlider ? itemStyle.thumbConnectorMargin : 0)
            rightMargin : (isVertical) ? 0 : (isThumbAboveSlider ? 0 : itemStyle.thumbConnectorMargin)
            topMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? itemStyle.thumbConnectorMargin : 0)
            bottomMargin : (!isVertical) ? 0 : (isThumbAboveSlider ? 0 : itemStyle.thumbConnectorMargin)
        }
        color: itemStyle.thumbConnectorColor
        opacity: thumb.shown ? 1.0 : 0.0
        Behavior on opacity {animation: itemStyle.thumbConnectorFading}
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
        enabled: isScrollable && isActive
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
                scrollOnePageForward()
            else if (inThumbTop)
                scrollOnePageBackward()
        }

        // Dragging behaviour
        function resetDrag() {
            dragYStart = drag.target.y
            thumbYStart = thumb.y
            sliderYStart = slider.y
            dragXStart = drag.target.x
            thumbXStart = thumb.x
            sliderXStart = slider.x
        }

        property int sliderYStart
        property int thumbYStart
        property int dragYStart
        property int dragYAmount: thumbArea.drag.target.y - thumbArea.dragYStart
        property int sliderXStart
        property int thumbXStart
        property int dragXStart
        property int dragXAmount: thumbArea.drag.target.x - thumbArea.dragXStart
        drag {
            target: Item {}
            axis: (isVertical) ? Drag.YAxis : Drag.XAxis
            filterChildren: true
            onActiveChanged: if (drag.active) resetDrag()
        }
        // update flickableItem's and thumb's position
        // cannot use Binding as there would be a binding loop
        onDragYAmountChanged: {
            var pos = clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, contentSize - pageSize);
            item.flickableItem.contentY = clamp(pos + item.flickableItem.originY, 0.0, contentSize - pageSize);
            thumb.y = clamp(thumbArea.thumbYStart + thumbArea.dragYAmount, 0, thumb.maximumPos);
        }
        onDragXAmountChanged: {
            var pos = clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, contentSize - pageSize);
            item.flickableItem.contentX = clamp(pos + item.flickableItem.originX, 0.0, contentSize - pageSize);
            thumb.x = clamp(thumbArea.thumbXStart + thumbArea.dragXAmount, 0, thumb.maximumPos);
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

        enabled: isActive

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
                    y = clamp(mouseY - thumb.height / 2, 0, thumb.maximumPos);
                } else {
                    var mouseX = proximityArea.containsMouse ? proximityArea.mouseX : thumbArea.mouseX;
                    x = clamp(mouseX - thumb.width / 2, 0, thumb.maximumPos);
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
            positionAnimation.to = clamp(diff, 0, maximumPos);
            positionAnimation.restart();
        }

        function placeThumbRearUnderMouse(mouse) {
            var diff = (isVertical) ? mouse.y - height * 3 / 4 : mouse.x - width * 3 / 4;
            positionAnimation.to = clamp(diff, 0, maximumPos);
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
        Behavior on opacity {animation: itemStyle.thumbFading}

        Flow {
            // disable mirroring as thumbs are placed in the same way no matter of RTL or LTR
            LayoutMirroring.enabled: false
            flow: (isVertical) ? Flow.TopToBottom : Flow.LeftToRight
            Image {
                id: thumbTop
                source: thumbArea.inThumbTop && thumbArea.pressed ? itemStyle.backwardThumbPressed : itemStyle.backwardThumbReleased
            }
            Image {
                id: thumbBottom
                source: thumbArea.inThumbBottom && thumbArea.pressed ? itemStyle.forwardThumbPressed : itemStyle.forwardThumbReleased
            }
        }
    }
}
