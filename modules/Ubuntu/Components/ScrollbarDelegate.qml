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
import Ubuntu.Components 0.1 as Theming

Item {
    id: visuals

    // delegate specific properties
    property real contentPosition
    property QtObject listView: null

    // common logic for Flickable and ListView to update contentPosition when Flicked
    Connections {
        target: item.flickableItem
        onContentYChanged: visuals.contentPosition = item.flickableItem.contentY - item.flickableItem.originY
        onContentXChanged: visuals.contentPosition = item.flickableItem.contentX - item.flickableItem.originX
    }

    /*
    Connections {
        target: visuals
        onContentPositionChanged: updateFlickableContent(visuals.contentPosition)
    }
    */

    function updateFlickableContent(position)
    {
        var newValue = 0;
        if (item.orientation == Qt.Vertical) {
            newValue = (!listView) ? position : position// - item.flickableItem.originY;
            //item.flickableItem.contentY = newValue
        } else {
            newValue = (!listView) ? position : position// - item.flickableItem.originX;
            //item.flickableItem.contentX = newValue
        }
        return newValue
    }

    // logic for ListView
    Component {
        id: listViewLogic
        Object {
            VisualDataModel {
                id: sectionModel
                model: item.flickableItem.section.delegate != undefined ? item.flickableItem.model : null
                delegate: Item {}
            }

            Theming.ModelSectionCounter {
                id: sectionCounter
                model: item.flickableItem.section.delegate != undefined ? sectionModel : null
                sectionProperty: item.flickableItem.section.property
                sectionCriteria: item.flickableItem.section.criteria
                onSectionCountChanged: {
                    print(item.internals.contentSize+", "+sectionCount + ", "+sectionProperty+", "+model.model)
                }
            }

            function delegateHeight(delegate)
            {
                // FIXME: this causes QML warnings because of unknown roles,
                // but we need it for correct content calculations
                if (delegate) {
                    var instance = delegate.createObject(null);
                    var ret = instance.height;
                    instance.destroy();
                    return ret;
                }

                return 0;
            }
            property int itemHeight: delegateHeight(item.flickableItem.delegate)
            property int sectionHeight: delegateHeight(item.flickableItem.section.delegate)
            property int spacingSize: item.flickableItem.spacing * (item.flickableItem.count - 1)
            property int itemsSize: item.flickableItem.count * itemHeight

            Binding {
                target: item.internals
                property: "contentSize"
                value: sectionCounter.sectionCount * sectionHeight
                       + item.flickableItem.count * itemHeight
                       + item.flickableItem.spacing * (item.flickableItem.count - 1)
            }

        }
    }


    Component.onCompleted: detectFlickableLogic()
    Connections {
        target: item
        onFlickableItemChanged: detectFlickableLogic()
    }

    function detectFlickableLogic()
    {
        if (item.flickableItem) {
            if (item.flickableItem.hasOwnProperty("header")) {
                // we consider Grids as Flickables?
                if (item.flickableItem.hasOwnProperty("cellWidth")) {
                    loadFlickableLogic();
                } else {
                    loadListViewLogic();
                }
            } else
                loadFlickableLogic();
        }
    }

    function loadFlickableLogic()
    {

    }

    function loadListViewLogic()
    {
        if (listView)
            listView.destroy()
        listView = listViewLogic.createObject(visuals)
    }

    /*****************************************
      Visuals
     *****************************************/
    anchors.fill: parent

    opacity: item.internals.scrollable ? 1.0 : 0.0
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
        var destination = contentPosition + amount
        scrollAnimation.to = clamp(destination, 0, item.internals.contentSize - item.internals.pageSize)
        scrollAnimation.restart()
    }

    function scrollOnePageBackward() {
        scrollBy(-item.internals.pageSize)
    }

    function scrollOnePageForward() {
        scrollBy(item.internals.pageSize)
    }

    function mapToPoint(map)
    {
        return Qt.point(map.x, map.y)
    }

    SmoothedAnimation {
        id: scrollAnimation

        duration: 200
        easing.type: Easing.InOutQuad
        //target: visuals
        //property: "contentPosition"
        target: item.flickableItem
        property: (item.orientation == Qt.Vertical) ? "contentY" : "contentX"
    }

    // represents the visible area of the scrollbar where slider and thumb connector are placed
    Item {
        id: scrollbarArea
        property real thickness: units.dp(2)
        property real proximityThickness: (item.orientation == Qt.Vertical) ? item.width - thickness : item.height - thickness
        anchors {
            fill: parent
            leftMargin: ((item.orientation == Qt.Horizontal) || (item.orientation == Qt.Vertical && item.align == "front"))
                        ? 0 : proximityThickness
            rightMargin: ((item.orientation == Qt.Horizontal) || (item.orientation == Qt.Vertical && item.align == "rear"))
                        ? 0 : proximityThickness
            topMargin: ((item.orientation == Qt.Vertical) || (item.orientation == Qt.Horizontal && item.align == "top"))
                        ? 0 : proximityThickness
            bottomMargin: ((item.orientation == Qt.Vertical) || (item.orientation == Qt.Horizontal && item.align == "bottom"))
                        ? 0 : proximityThickness
        }
    }
    // The thumb appears whenever the mouse gets close enough to the scrollbar
    // and disappears after being for a long enough time far enough of it
    MouseArea {
        id: proximityArea

        anchors {
            fill: parent
            leftMargin: (item.orientation == Qt.Horizontal)  ? 0 : ((item.align == "front") ? scrollbarArea.thickness : 0)
            rightMargin: (item.orientation == Qt.Horizontal) ? 0 : ((item.align == "rear") ? scrollbarArea.thickness : 0)
            topMargin: (item.orientation == Qt.Vertical) ? 0 : ((item.align == "top") ? scrollbarArea.thickness : 0)
            bottomMargin: (item.orientation == Qt.Vertical) ? 0 : ((item.align == "bottom") ? scrollbarArea.thickness : 0)
        }
        enabled: item.internals.scrollable
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
            left: ((item.orientation == Qt.Vertical) && (item.align == "front"))
                  ? scrollbarArea.left : undefined
            right: ((item.orientation == Qt.Vertical) && (item.align == "rear"))
                   ? scrollbarArea.right : undefined
            top: ((item.orientation == Qt.Horizontal) && (item.align == "top"))
                 ? scrollbarArea.top : undefined
            bottom: ((item.orientation == Qt.Horizontal) && (item.align == "bottom"))
                    ? scrollbarArea.bottom : undefined
        }

        property int minimalSize: units.gu(5)
        width: (item.orientation == Qt.Vertical) ?
                   scrollbarArea.thickness :
                   clamp(item.internals.pageSize / item.internals.contentSize * item.width, minimalSize, item.width)
        height: (item.orientation == Qt.Horizontal) ?
                    scrollbarArea.thickness :
                    clamp(item.internals.pageSize / item.internals.contentSize * item.height, minimalSize, item.height)

        Behavior on width {
            enabled: (item.orientation == Qt.Horizontal)
            animation: itemStyle.sliderAnimation
        }
        Behavior on height {
            enabled: (item.orientation == Qt.Vertical)
            animation: itemStyle.sliderAnimation
        }

        Binding {
            when: (item.orientation == Qt.Vertical)
            target: slider
            property: "y"
            value: clampAndProject(contentPosition, 0.0, item.internals.contentSize - item.internals.pageSize, 0.0, item.height - slider.height)
        }
        Binding {
            when: (item.orientation == Qt.Horizontal)
            target: slider
            property: "x"
            value: clampAndProject(contentPosition, 0.0, item.internals.contentSize - item.internals.pageSize, 0.0, item.width - slider.width)
        }
    }

    // The sliderThumbConnector ensures a visual connection between the slider and the thumb
    Rectangle {
        id: sliderThumbConnector

        property bool isThumbAboveSlider: (item.orientation == Qt.Vertical)
                                          ? thumb.y < slider.y : thumb.x < slider.x
        anchors {
            left: (item.orientation == Qt.Vertical)
                  ? scrollbarArea.left : (isThumbAboveSlider ? thumb.left : slider.right)
            leftMargin : (item.orientation == Qt.Vertical)
                         ? 0 : (isThumbAboveSlider ? units.dp(3) : 0)
            right: (item.orientation == Qt.Vertical)
                  ? scrollbarArea.right : (isThumbAboveSlider ? slider.left : thumb.right)
            rightMargin : (item.orientation == Qt.Vertical)
                         ? 0 : (isThumbAboveSlider ? 0 : units.dp(6))
            top: (item.orientation == Qt.Vertical)
                 ? (isThumbAboveSlider ? thumb.top : slider.bottom) : scrollbarArea.top
            topMargin : (item.orientation == Qt.Vertical)
                         ? (isThumbAboveSlider ? units.dp(3) : 0) : 0
            bottom: (item.orientation == Qt.Vertical)
                    ? (isThumbAboveSlider ? slider.top : thumb.bottom) : scrollbarArea.bottom
            bottomMargin : (item.orientation == Qt.Vertical)
                         ? (isThumbAboveSlider ? 0 : units.dp(6)) : 0
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
            leftMargin: (item.orientation == Qt.Horizontal)  ? 0 : ((item.align == "front") ? 0 : units.dp(-2) - thumb.width)
            rightMargin: (item.orientation == Qt.Horizontal) ? 0 : ((item.align == "rear") ? 0 : units.dp(-2) - thumb.width)
            topMargin: (item.orientation == Qt.Vertical) ? 0 : ((item.align == "top") ?  0 : units.dp(-2) - thumb.height)
            bottomMargin: (item.orientation == Qt.Vertical) ? 0 : ((item.align == "bottom") ?  0 : units.dp(-2) - thumb.height)
        }
        enabled: item.internals.scrollable
        hoverEnabled: true
        onEntered: thumb.show()
        onPressed: {
            if (item.orientation == Qt.Vertical) {
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

        property int dragYStart
        property int dragXStart
        property int dragYAmount: thumbArea.drag.target.y - thumbArea.dragYStart
        property int dragXAmount: thumbArea.drag.target.x - thumbArea.dragXStart
        property int thumbYStart
        property int sliderYStart
        property int thumbXStart
        property int sliderXStart
        drag {
            target: Item {}
            axis: (item.orientation == Qt.Vertical) ? Drag.YAxis : Drag.XAxis
            filterChildren: true
            onActiveChanged: if (drag.active) resetDrag()
        }
        //onDragYAmountChanged: item.flickableItem.contentY = updateFlickableContent(clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, item.internals.contentSize - item.internals.pageSize))
        //onDragXAmountChanged: item.flickableItem.contentX = updateFlickableContent(clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, item.internals.contentSize - item.internals.pageSize))

        //onDragYAmountChanged: item.flickableItem.contentY = clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, item.internals.contentSize - item.internals.pageSize) - item.flickableItem.originY
        //onDragXAmountChanged: item.flickableItem.contentX = clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, item.internals.contentSize - item.internals.pageSize)  - item.flickableItem.originX
        onDragYAmountChanged: {
            contentPosition = clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, item.internals.contentSize - item.internals.pageSize)
            item.flickableItem.contentY = contentPosition + item.flickableItem.originY
            print(item.flickableItem.contentY+", origin: "+item.flickableItem.originY)
        }
        onDragXAmountChanged: {
            contentPosition = clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, item.internals.contentSize - item.internals.pageSize)
            item.flickableItem.contentX = contentPosition// - item.flickableItem.originX
        }

        // The thumb allows the user to scroll the entire content, from top to bottom/left to right
        /*
        Binding {
            target: item.flickableItem
            property: (item.orientation == Qt.Vertical) ? "contentY" : "contentX"
            when: thumbArea.drag.active
            value: (item.orientation == Qt.Vertical) ?
                       clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, item.internals.contentSize - item.internals.pageSize) - item.flickableItem.originY:
                       clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, item.internals.contentSize - item.internals.pageSize)  - item.flickableItem.originX
        }
        */

        Binding {
            target: thumb
            property: (item.orientation == Qt.Vertical) ? "y" : "x"
            value: (item.orientation == Qt.Vertical) ?
                       clamp(thumbArea.thumbYStart + thumbArea.dragYAmount, 0, thumb.maximumPos) :
                       clamp(thumbArea.thumbXStart + thumbArea.dragXAmount, 0, thumb.maximumPos)
            when: thumbArea.drag.active
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

        anchors {
            left: (item.orientation == Qt.Vertical) ? item.internals.leftAnchor(slider) : undefined
            right: (item.orientation == Qt.Vertical) ? item.internals.rightAnchor(slider) : undefined
            top: (item.orientation == Qt.Horizontal) ? item.internals.topAnchor(slider) : undefined
            bottom: (item.orientation == Qt.Horizontal) ? item.internals.bottomAnchor(slider) : undefined
        }

        width: childrenRect.width
        height: childrenRect.height

        property bool shown
        property int maximumPos: (item.orientation == Qt.Vertical) ?
                                     item.height - thumb.height :
                                     item.width - thumb.width

        /* Show the thumb as close as possible to the mouse pointer */
        onShownChanged: {
            if (shown) {
                if (item.orientation == Qt.Vertical) {
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
            var diff = (item.orientation == Qt.Vertical) ? mouse.y - height / 4 : mouse.x - width / 4;
            positionAnimation.to = clamp(diff, 0, maximumPos);
            positionAnimation.restart();
        }

        function placeThumbRearUnderMouse(mouse) {
            var diff = (item.orientation == Qt.Vertical) ? mouse.y - height * 3 / 4 : mouse.x - width * 3 / 4;
            positionAnimation.to = clamp(diff, 0, maximumPos);
            positionAnimation.restart();
        }

        NumberAnimation {
            id: positionAnimation

            duration: 100
            easing.type: Easing.InOutQuad
            target: thumb
            property: (item.orientation == Qt.Vertical) ? "y" : "x"
        }

        opacity: shown ? (thumbArea.containsMouse || thumbArea.drag.active ? 1.0 : 0.5) : 0.0
        Behavior on opacity {animation: itemStyle.thumbFading}

        Flow {
            // disable mirroring as thumbs are placed in the same way no matter of RTL or LTR
            LayoutMirroring.enabled: false
            flow: (item.orientation == Qt.Vertical) ? Flow.TopToBottom : Flow.LeftToRight
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
