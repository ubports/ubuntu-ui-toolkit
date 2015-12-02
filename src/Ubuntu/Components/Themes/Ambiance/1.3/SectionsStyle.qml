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
import QtGraphicalEffects 1.0

Item {
    id: sectionsStyle

    //VISUAL CHANGES:
    //- sectionColor
    //- added underlineColor
    //- fontsize to medium
    //- font weight to Light

    implicitWidth: units.gu(50) //sectionsRow.width
    implicitHeight: units.gu(4)

    enabled: styledItem.enabled
    opacity: enabled ? 1.0 : 0.5
    /*!
      The foreground color of unselected sections.
     */
    //FIXME: hardcoded color
    property color sectionColor: "#888888"//theme.palette.selected.backgroundText

    /*!
      The foreground color of underline rectangle of unselected sections.
     */
    //FIXME: hardcoded color
    property color underlineColor: Qt.rgba(0,0,0,0.2)//theme.palette.selected.backgroundText

    /*!
      The foreground color of the selected section.
     */
    property color selectedSectionColor: UbuntuColors.orange

    /*!
      The background color for the pressed section button.
     */
    property color pressedBackgroundColor: theme.palette.selected.background

    /*!
      The font size for the text in the buttons.
     */
    property string fontSize: "medium"

    /*!
      The spacing on the left and right sides of the label
      inside a section button.
     */
    property real horizontalLabelSpacing: units.gu(2)

    /*!
      The height of the bar underlining the sections.
     */
    property real underlineHeight: units.dp(2)

    property real __listViewMargin: units.gu(3)
    property bool __hoveringLeft: false
    property bool __hoveringRight: false

    //we don't clip listview on purpose, so we have to clip here to prevent Sections element
    //from painting outside its area
    clip: true

    //This item is needed for the OpacityMask feature. It is needed to make sure that when we
    //bring a list element into view, that element won't be covered by the opacity mask. So we
    //disable clipping on the list but we give it margins. This way when an item is repositioned
    //to be within the listview, that item will not be positioned under the opacity mask. (which is
    //what would have happened if the listview were filling the parent)
    Item {
        id: listviewcontainer
        anchors.fill: parent

        //We need to set this to 0.0 when OpacityMask will draw this listview for us.
        //we don't set visible: false because we still want to get the input events!
        opacity: 1.0

        ListView {
            id: sectionsListView
            objectName: "section_listview"

            property bool animateContentX: false

            function ensureItemIsInTheMiddle(currentItem) {
                if (currentItem !== null) {
                    //stop the flick before doing computations
                    if (moving) {
                        return
                    }
                    if (dragging || flicking) {
                        cancelFlick()
                    }

                    console.log("CONTENTX", contentX)
                    if (contentXAnim.running) { contentXAnim.stop() }
                    console.log("CONTENTXAFTER", contentX)

                    var pos =  currentItem.mapToItem(sectionsListView.contentItem, 0, 0)

                    //In the case where we're moving to an item which is outside of the screen (this
                    //happens when using Keyboard navigation after scrolling the view), we
                    //try working around the fact that contentWidth/X keeps changing
                    //by using listview's functions (WHICH DON'T HAVE ANIMATIONS though) just to position the
                    //view more or less at the right place. After that, we'll fake an animation from one side to the middle of the
                    //screen.
                    /*if (pos.x + pos.width <= sectionsListView.x) {
                        positionViewAtIndex(currentIndex, ListView.Beginning)
                        //refresh the mapping as we've moved the view
                        pos = currentItem.mapToItem(sectionsListView, 0, 0)

                    } else if (pos.x >= sectionsListView.x + sectionsListView.width) {
                        positionViewAtIndex(currentIndex, ListView.End)
                        //refresh the mapping as we've moved the view
                        pos = currentItem.mapToItem(sectionsListView, 0, 0)
                    }*/

                    var newContentX = pos.x - sectionsListView.width/2 + currentItem.width/2
                    contentXAnim.from = contentX
                    //make sure we don't overshoot bounds
                    contentXAnim.to =  Math.max(originX, Math.min(newContentX, originX + contentWidth - width))
                    contentXAnim.start()

                    console.log("OUR VALUES", contentX, originX, contentWidth, contentXAnim.to)
                    //FIXME: due to listview internal implementation and the fact that delegates don't have a fixed size,
                    //this breaks in some cases, due to originX changing *after* we start the
                    //animation. In those cases the current item doesn't align to the horizontal center because we compute
                    //contentXAnim.to based on an originX which is then changed by the internals after we have already
                    //started the animation. We could fixup the position at the end of the animation,
                    //but that would just be a workaround.
                }
            }

            anchors {
                fill: parent
                leftMargin: __listViewMargin
                rightMargin: __listViewMargin
            }

            //this is just to disable keyboard navigation to avoid messing with contentX/contentWidth while
            //the view is moving
            focus: !moving
            onContentXChanged: console.log(contentX, originX, contentWidth)

            //FIXME: this means when we resize the window it will refocus on the current item even if it's outside of the view!
            //NOTE: removing this also breaks the alignment when the sections are initialized, because of contentX/contentWidth changing
            onWidthChanged: ensureItemIsInTheMiddle(currentItem)

            orientation: ListView.Horizontal

            //We want to block dragging but at the same time we want the keyboard to work!!!
            //interactive: contentWidth > width
            boundsBehavior: Flickable.StopAtBounds

            model: styledItem.model

            //We need this to make sure that we have delegates for the whole width, since we have
            //clip disabled. Read more
            displayMarginBeginning: __listViewMargin
            displayMarginEnd: __listViewMargin

            //make sure that the currentItem is in the middle when everything is initialized
            Component.onCompleted: ensureItemIsInTheMiddle(currentItem)

            //FIXME: keyboard navigation offered by ListView will break this, won't it?
            currentIndex: styledItem.selectedIndex
            onCurrentIndexChanged: {
                styledItem.selectedIndex = currentIndex
            }
            onCurrentItemChanged: {
                //adjust contentX so that the item is kept in the middle
                //don't use ListView.ApplyRange because that does an awkward animation when you select an item
                //*while* the current item is outside of screen
                ensureItemIsInTheMiddle(currentItem)
            }

            //highlightMoveDuration: UbuntuAnimation.BriskDuration
            //highlightResizeDuration: UbuntuAnimation.BriskDuration

            highlightFollowsCurrentItem: false
            //DON'T use ApplyRange, because it will cause a weird animation when you select an item which is on screen
            //with the previously selected item being out of screen
            //highlightRangeMode: ListView.ApplyRange
            //preferredHighlightEnd: width/2 + currentItem.width/2
            //preferredHighlightBegin: width/2 - currentItem.width/2

            //highlightFollowsCurrentItem will resize the highlight element to fill the delegate,
            //so we need an Item in the middle to set a height for the highlight rectangle different
            //from the delegate size
            highlight: Item {
                //show the highlight on top of the delegate
                z: 2

                x: sectionsListView.currentItem ? sectionsListView.currentItem.x : -width
                width: sectionsListView.currentItem ? sectionsListView.currentItem.width : 0
                height: sectionsListView.currentItem ? sectionsListView.currentItem.height : 0

                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: sectionsStyle.underlineHeight
                    color: sectionsStyle.selectedSectionColor
                }

                Behavior on x { UbuntuNumberAnimation {} }
            }

            delegate: AbstractButton {
                id: sectionButton
                objectName: "section_button_" + index
                width: label.width + 2 * sectionsStyle.horizontalLabelSpacing
                height: sectionsStyle.height
                property bool selected: index === styledItem.selectedIndex
                onClicked: { styledItem.selectedIndex = index; sectionsListView.forceActiveFocus() }

                // Background pressed highlight
                Rectangle {
                    visible: parent.pressed
                    anchors.fill: parent
                    color: sectionsStyle.pressedBackgroundColor
                }

                // Section title
                Label {
                    id: label
                    objectName: "section_button_label_" + index
                    // modelData may be either a string, or an Action
                    text: modelData.hasOwnProperty("text") ? modelData.text : modelData
                    fontSize: sectionsStyle.fontSize
                    font.weight: Font.Light
                    anchors.centerIn: parent
                    color: sectionButton.selected ?
                               sectionsStyle.selectedSectionColor :
                               sectionsStyle.sectionColor

                    //FIXME: color animation? is that ok to design? what's the duration?
                    Behavior on color { ColorAnimation { duration: 500 } }
                }

                // Section title underline
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: sectionsStyle.underlineHeight
                    color: sectionsStyle.underlineColor
                }
            }

            SmoothedAnimation {
                id: contentXAnim
                target: sectionsListView
                property: "contentX"
                duration: UbuntuAnimation.FastDuration
                velocity: units.gu(10)
                //alwaysRunToEnd: true
            }

            //Behavior on contentX { SmoothedAnimation { duration: UbuntuAnimation.FastDuration; velocity: units.gu(10)} }

        }
    }

    MouseArea {
        id: hoveringArea

        property real iconsDisabledOpacity: 0.3

        function checkHovering(mouse) {
            if (mouse.x < __listViewMargin) {
                if (!__hoveringLeft) __hoveringLeft = true
            } else if (mouse.x > width - __listViewMargin ) {
                if (!__hoveringRight) __hoveringRight = true
            } else {
                __hoveringLeft = false
                __hoveringRight = false
            }
        }

        onContainsMouseChanged: console.log(containsMouse)
        anchors.fill: parent
        hoverEnabled: true

        onPositionChanged: checkHovering(mouse)
        onExited: {
            __hoveringLeft = false
            __hoveringRight = false
        }
        onPressed: if (!__hoveringLeft && !__hoveringRight) {
                       mouse.accepted = false
                   }
        onClicked: {
            //scroll the list to bring the element which is under the cursor into the view
            //var item = sectionsListView.itemAt(mouse.x + sectionsListView.contentX - __listViewMargin, mouse.y)
            //if (item !== null) {
            //We could use positionViewAtIndex(...) here but it wouldn't provide animation

            if (contentXAnim.running) contentXAnim.stop()

            //Scroll one item at a time with animation
            //sectionsListView.contentX = __hoveringLeft ? item.mapToItem(sectionsListView.contentItem, 0,0).x : item.mapToItem(sectionsListView.contentItem, 0,0).x - sectionsListView.width + item.width
            var newContentX = sectionsListView.contentX + (sectionsListView.width * (__hoveringLeft ? -1 : 1))

            contentXAnim.from = sectionsListView.contentX
            //make sure we don't overshoot bounds
            contentXAnim.to = Math.max(sectionsListView.originX, Math.min(newContentX, sectionsListView.originX + sectionsListView.contentWidth - sectionsListView.width))

            contentXAnim.start()

            //}
        }

        Icon {
            id: leftHoveringIcon
            anchors.left: parent.left
            anchors.leftMargin: units.gu(1)
            anchors.verticalCenter: parent.verticalCenter
            width: units.gu(1)
            height: units.gu(1)
            visible: false
            rotation: 180
            opacity: sectionsListView.atXBeginning ? hoveringArea.iconsDisabledOpacity : 1.0
            name: "chevron"
        }

        Icon {
            id: rightHoveringIcon
            anchors.right: parent.right
            anchors.rightMargin: units.gu(1)
            anchors.verticalCenter: parent.verticalCenter
            width: units.gu(1)
            height: units.gu(1)
            visible: false
            opacity: sectionsListView.atXEnd ? hoveringArea.iconsDisabledOpacity : 1.0
            name: "chevron"
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: units.dp(1)
        color: sectionsStyle.underlineColor
    }

    LinearGradient {
        id: gradient

        visible: false
        anchors.fill: parent
        start: Qt.point(0,0)
        end: Qt.point(width,0)

        property real __gradientWidth: units.gu(3) / gradient.width
        //the width is __gradientWidth, but we want the gradient to actually start/finish at __gradientSplitPosition
        //just to leave some margin.
        property real __gradientSplitPosition: 3/4 * __gradientWidth

        gradient: Gradient {
            //left gradient
            GradientStop { position: 0.0 ; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: gradient.__gradientSplitPosition ; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: gradient.__gradientWidth; color: Qt.rgba(1,1,1,1) }
            //right gradient
            GradientStop { position: 1.0 - gradient.__gradientWidth; color: Qt.rgba(1,1,1,1) }
            GradientStop { position: 1.0 - gradient.__gradientSplitPosition; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: 1.0; color: Qt.rgba(1,1,1,0) }
        }

    }

    OpacityMask {
        id: mask
        anchors.fill: parent

        visible: false
        source: listviewcontainer
        maskSource: gradient
    }

    //Since we only show one arrow at a time, let's reuse the same item and handle the property changes with states
    states: [
        State {
            name: "hovering"
            when: hoveringArea.containsMouse
            PropertyChanges { target: mask; visible: true }
            PropertyChanges { target: listviewcontainer; opacity: 0.0 }
            PropertyChanges { target: leftHoveringIcon; visible: true; }
            PropertyChanges { target: rightHoveringIcon; visible: true; }
        }
    ]
}
