/*
 * Copyright 2016 Canonical Ltd.
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

    implicitWidth: sectionsListView.contentWidth + 2 * listViewContainer.listViewMargins
    implicitHeight: units.gu(4)

    /*!
      The foreground color of unselected sections.
     */
    property color sectionColor: enabled
                                 ? theme.palette.normal.backgroundTertiaryText
                                 : theme.palette.disabled.backgroundTertiaryText

    /*!
      The foreground color of underline rectangle of unselected sections.
     */
    property color underlineColor: theme.palette.normal.base

    /*!
      The foreground color of the selected section.
     */
    property color selectedSectionColor: enabled
                                         ? theme.palette.selected.backgroundTertiaryText
                                         : theme.palette.selectedDisabled.backgroundTertiaryText

    /*!
      The background color for the pressed section button.
     */
    property color pressedBackgroundColor: theme.palette.highlighted.background

    /*!
      The for the text in the buttons.
     */
    property int textSize: Label.Medium

    /*!
      The spacing on the left and right sides of the label
      inside a section button.
     */
    property real horizontalLabelSpacing: units.gu(2)

    /*!
      The height of the bar underlining the sections.
     */
    property real underlineHeight: units.dp(2)

    // We don't clip listview on purpose, so we have to clip here to prevent Sections element
    // from painting outside its area.
    clip: true

    //This item is needed for the OpacityMask feature. It is needed to make sure that when we
    //bring a list element into view, that element won't be covered by the opacity mask. So we
    //disable clipping on the list but we give it margins. This way when an item is repositioned
    //to be within the listview, that item will not be positioned under the opacity mask. (which is
    //what would have happened if the listview were filling the parent)
    Item {
        id: listViewContainer
        anchors.fill: parent

        property real listViewMargins: units.gu(3)

        //We need to set this to 0.0 when OpacityMask will draw this listview for us.
        //we don't set visible: false because we still want to get the input events!
        opacity: 1.0

        ListView {
            id: sectionsListView
            objectName: "sections_listview"

            property bool animateContentX: false

            function ensureItemIsInTheMiddle(item) {
                if (item !== null) {
                    //stop the flick before doing computations
                    if (moving) {
                        return;
                    }
                    if (dragging || flicking) {
                        cancelFlick();
                    }
                    if (contentXAnim.running) {
                        contentXAnim.stop();
                    }

                    var pos = item.mapToItem(sectionsListView.contentItem, 0, 0);
                    var newContentX = pos.x - sectionsListView.width/2 + item.width/2;
                    contentXAnim.from = contentX;
                    //make sure we don't overshoot bounds
//                    contentXAnim.to =  Math.max(originX, Math.min(newContentX, originX + contentWidth - width));
                    contentXAnim.to = MathUtils.clamp(newContentX, originX, originX + contentWidth - width);
                    contentXAnim.start();
                }
            }

            anchors {
                fill: parent
                leftMargin: listViewContainer.listViewMargins
                rightMargin: listViewContainer.listViewMargins
            }

            //this is just to disable keyboard navigation to avoid messing with contentX/contentWidth while
            //the view is moving
            focus: !moving

            onWidthChanged: ensureItemIsInTheMiddle(currentItem)
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds

            model: styledItem.model

            //We need this to make sure that we have delegates for the whole width, since we have
            //clip disabled.
            displayMarginBeginning: listViewContainer.listViewMargins
            displayMarginEnd: listViewContainer.listViewMargins

            //make sure that the currentItem is in the middle when everything is initialized
            Component.onCompleted: ensureItemIsInTheMiddle(currentItem)

            //FIXME: keyboard navigation offered by ListView will break this, won't it?
            currentIndex: styledItem.selectedIndex
            onCurrentIndexChanged: {
                styledItem.selectedIndex = currentIndex;
            }
            onCurrentItemChanged: {
                //adjust contentX so that the item is kept in the middle
                //don't use ListView.ApplyRange because that does an awkward animation when you select an item
                //*while* the current item is outside of screen
                ensureItemIsInTheMiddle(currentItem);
            }

            highlightFollowsCurrentItem: false
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
                    textSize: sectionsStyle.textSize
                    font.weight: Font.Light
                    anchors {
                        baseline: underline.bottom
                        baselineOffset: -units.gu(2)
                        horizontalCenter: parent.horizontalCenter
                    }

                    color: sectionButton.selected ?
                               sectionsStyle.selectedSectionColor :
                               sectionsStyle.sectionColor

                    //FIXME: color animation? is that ok to design? what's the duration?
                    Behavior on color { ColorAnimation { duration: 500 } }
                }

                // Section title underline
                Rectangle {
                    id: underline
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
            }
        }
    }

    MouseArea {
        // Detect hovering over the left and right areas to show the scrolling chevrons.
        id: hoveringArea

        property real iconsDisabledOpacity: 0.3

        property bool hoveringLeft: false
        property bool hoveringRight: false

        function checkHovering(mouse) {
            if (mouse.x < listViewContainer.listViewMargins) {
                if (!hoveringLeft) hoveringLeft = true;
            } else if (mouse.x > width - listViewContainer.listViewMargins) {
                if (!hoveringRight) hoveringRight = true;
            } else {
                hoveringLeft = false;
                hoveringRight = false;
            }
        }

        anchors.fill: parent
        hoverEnabled: true

        onPositionChanged: checkHovering(mouse)
        onExited: {
            hoveringLeft = false;
            hoveringRight = false;
        }
        onPressed: {
            if (!hoveringLeft && !hoveringRight) {
                mouse.accepted = false;
            }
        }
        onClicked: {
            // positionViewAtIndex() does not provide animation
            if (contentXAnim.running) contentXAnim.stop();
            var newContentX = sectionsListView.contentX + (sectionsListView.width * (hoveringLeft ? -1 : 1))
            contentXAnim.from = sectionsListView.contentX;
            // make sure we don't overshoot bounds
//            contentXAnim.to = Math.max(sectionsListView.originX, Math.min(newContentX, sectionsListView.originX + sectionsListView.contentWidth - sectionsListView.width))
            contentXAnim.to = MathUtils.clamp(newContentX, sectionsListView.originX, sectionsListView.originX + sectionsListView.contentWidth - sectionsListView.width);
            contentXAnim.start();
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
        source: listViewContainer
        maskSource: gradient
    }

    states: [
        State {
            name: "hovering"
            when: hoveringArea.containsMouse
            PropertyChanges { target: mask; visible: true }
            PropertyChanges { target: listViewContainer; opacity: 0.0 }
            PropertyChanges { target: leftHoveringIcon; visible: true; }
            PropertyChanges { target: rightHoveringIcon; visible: true; }
        }
    ]
}
