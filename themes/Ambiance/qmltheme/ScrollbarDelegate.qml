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
  */

Item {
    id: visuals
    // helper properties to ease code readability
    property Flickable flickableItem: item.flickableItem
    property bool interactive: item.__interactive
    property bool isScrollable: item.__private.scrollable && pageSize > 0.0
                                && contentSize > 0.0 && contentSize > pageSize
    property bool isVertical: (item.__private.vertical)
    property bool frontAligned: (item.align === Qt.AlignLeading)
    property bool rearAligned: (item.align === Qt.AlignTrailing)
    property bool topAligned: (item.align === Qt.AlignTop)
    property bool bottomAligned: (item.align === Qt.AlignBottom)

    property real pageSize: (isVertical) ? item.height : item.width
    property real contentSize: (listView) ?
                                   listView.size :
                                   ((isVertical) ? item.flickableItem.contentHeight : item.flickableItem.contentWidth)
    property real overlayOpacityWhenShown: StyleUtils.itemStyleProperty("overlayOpacityWhenShown", 0.6)
    property real overlayOpacityWhenHidden: StyleUtils.itemStyleProperty("overlayOpacityWhenHidden", 0.0)
    property bool overlay: StyleUtils.itemStyleProperty("overlay", false) && !interactive

    property real contentPosition
    property QtObject listView: logicLoader.item

    /* Removing the first row of the ListView's model will render
       ListView.contentY invalid and therefore break the scrollbar's position.
       This is fixable in QtQuick 2.0 thanks to the introduction of the
       Flickable.originY property, however the property is not reliable enough.
       Therefore we compute originY manually using the fact that
       ListView.visibleArea.yPosition is not rendered invalid by removing the
       first row of the ListView's model.
       Unfortunately the result is not flawless when the ListView uses section
       headers because ListView.visibleArea.yPosition is often slightly incorrect.

       Ref.: https://bugreports.qt-project.org/browse/QTBUG-20927
             https://bugreports.qt-project.org/browse/QTBUG-21358
             http://doc-snapshot.qt-project.org/5.0/qml-qtquick2-flickable.html#originX-prop
    */
    property real originX: (listView) ? -item.flickableItem.contentX + Math.round(item.flickableItem.visibleArea.xPosition * contentSize) : item.flickableItem.originX
    property real originY: (listView) ? -item.flickableItem.contentY + Math.round(item.flickableItem.visibleArea.yPosition * contentSize) : item.flickableItem.originY

    // common logic for Flickable and ListView to update contentPosition when Flicked
    Connections {
        target: item.flickableItem
        onContentYChanged: if (isVertical) contentPosition = MathUtils.clamp(item.flickableItem.contentY - visuals.originY, 0.0, contentSize)
        onContentXChanged: if (!isVertical) contentPosition = MathUtils.clamp(item.flickableItem.contentX - visuals.originX, 0.0, contentSize)
    }
    // logic for ListView
    Component {
        id: listViewLogic
        Object {
            /* ListView.contentHeight is not reliable when section headers are defined.
               In that case we compute 'size' manually.

               Ref.: https://bugreports.qt-project.org/browse/QTBUG-17057
                     https://bugreports.qt-project.org/browse/QTBUG-19941
            */
            property real size: sectionCounter.sectionCount * sectionHeight + itemsSize + spacingSize
            property int sectionHeight: sectionCounter.sectionHeight
            property int spacingSize: flickableItem.spacing * (flickableItem.count - 1)
            property int itemsSize: flickableItem.count * QuickUtils.modelDelegateHeight(flickableItem.delegate, flickableItem.model)

            // need to capture count change otherwise the count won't be
            // reported for the proxy models
            Connections {
                target: flickableItem
                onCountChanged: itemsSize = flickableItem.count * QuickUtils.modelDelegateHeight(flickableItem.delegate, flickableItem.model)
            }

            ModelSectionCounter {
                id: sectionCounter
                view: flickableItem
            }
        }
    }
    Loader { id:logicLoader }
    onFlickableItemChanged: {
        if (flickableItem) {
            if (flickableItem.hasOwnProperty("header")) {
                // we consider Grids same as Flickables
                if (flickableItem.hasOwnProperty("cellWidth")) {
                    logicLoader.sourceComponent = undefined;
                } else {
                    logicLoader.sourceComponent = listViewLogic;
                }
            } else
                logicLoader.sourceComponent = undefined;
        }
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
            }
        },
        Transition {
            from: '*'
            to: 'overlay'
            NumberAnimation {
                target: visuals
                property: "opacity"
                duration: StyleUtils.itemStyleProperty("scrollbarFadeInAnimation").duration
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
                    //to: overlayOpacityWhenHidden
                    duration: StyleUtils.itemStyleProperty("scrollbarFadeOutAnimation").duration
                    easing.type: StyleUtils.itemStyleProperty("scrollbarFadeOutAnimation").easing.type
                }
            }
        }
    ]

    /* Scroll by amount pixels never overshooting */
    function scrollBy(amount) {
        var destination = contentPosition + amount
        destination += (isVertical) ? visuals.originY : visuals.originX
        scrollAnimation.to = MathUtils.clamp(destination, 0, contentSize - pageSize)
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
    Rectangle {
        id: slider

        color: itemStyle.sliderColor

        anchors {
            left: (isVertical) ? scrollbarArea.left : undefined
            right: (isVertical) ? scrollbarArea.right : undefined
            top: (!isVertical) ? scrollbarArea.top : undefined
            bottom: (!isVertical) ? scrollbarArea.bottom : undefined
        }

        x: (isVertical) ? 0 : MathUtils.clampAndProject(contentPosition, 0.0, contentSize - pageSize, 0.0, item.width - slider.width)
        y: (!isVertical) ? 0 : MathUtils.clampAndProject(contentPosition, 0.0, contentSize - pageSize, 0.0, item.height - slider.height)
        width: (isVertical) ? scrollbarArea.thickness : sliderSizer.size
        height: (!isVertical) ? scrollbarArea.thickness : sliderSizer.size

        ScrollSliderSizer {
            id: sliderSizer
            positionRatio: (isVertical) ? item.flickableItem.visibleArea.yPosition : item.flickableItem.visibleArea.xPosition
            sizeRatio: (isVertical) ? item.flickableItem.visibleArea.heightRatio : item.flickableItem.visibleArea.widthRatio
            maximumPosition: (isVertical) ? item.flickableItem.height : item.flickableItem.width
            minimumSize: units.gu(2)
        }

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
            var pos = MathUtils.clampAndProject(thumbArea.sliderYStart + thumbArea.dragYAmount, 0.0, item.height - slider.height, 0.0, contentSize - pageSize);
            item.flickableItem.contentY = MathUtils.clamp(pos + visuals.originY, 0.0, contentSize - pageSize);
            thumb.y = MathUtils.clamp(thumbArea.thumbYStart + thumbArea.dragYAmount, 0, thumb.maximumPos);
        }
        onDragXAmountChanged: {
            var pos = MathUtils.clampAndProject(thumbArea.sliderXStart + thumbArea.dragXAmount, 0.0, item.width - slider.width, 0.0, contentSize - pageSize);
            item.flickableItem.contentX = MathUtils.clamp(pos + visuals.originX, 0.0, contentSize - pageSize);
            thumb.x = MathUtils.clamp(thumbArea.thumbXStart + thumbArea.dragXAmount, 0, thumb.maximumPos);
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
