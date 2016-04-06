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
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import QtGraphicalEffects 1.0
import Ubuntu.Components.Private 1.3

Item {
    id: sectionsStyle

    implicitWidth: sectionsListView.contentWidth + 2 * listViewContainer.listViewMargins
    implicitHeight: Screen.height > units.gu(50) ? units.gu(4) : units.gu(3)

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
      The size of text in the buttons.
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

        property real listViewMargins: units.gu(2)

        //We need to set this to 0.0 when OpacityMask will draw this listview for us.
        //we don't set visible: false because we still want to get the input events!
        opacity: 1.0

        ListView {
            id: sectionsListView
            objectName: "sections_listview"

            property bool animateContentX: false
            activeFocusOnTab: false // FIXME: Enable proper focus handling

            // Position the selected item correctly.
            // For a scrollable ListView, if the item was already fully visible,
            //  no repositioning is needed. If the item was (partially) invisible,
            //  position it so that it becomes fully visible.
            // If the ListView is not scrollable, the first item will be aligned with
            //  the left of the ListView.
            function positionItem(item) {
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

                    contentXAnim.from = contentX;
                    //make sure we don't overshoot bounds
                    if (sectionsListView.contentWidth <= sectionsListView.width) {
                        // No scrolling, position the sections on the left.
                        contentXAnim.to = originX;
                    } else {
                        // Position the selected Item so that it is fully visible.
                        contentXAnim.to = MathUtils.clamp(contentX, originX, originX + contentWidth - width);
                    }
                    if (contentXAnim.from !== contentXAnim.to) {
                        contentXAnim.start();
                    }
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

            onWidthChanged: positionItem(currentItem)
            //make sure that the currentItem is in the middle when everything is initialized
            Component.onCompleted: positionItem(currentItem)

            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds

            model: styledItem.model

            //We need this to make sure that we have delegates for the whole width, since we have
            //clip disabled.
            displayMarginBeginning: listViewContainer.listViewMargins
            displayMarginEnd: listViewContainer.listViewMargins

            currentIndex: styledItem.selectedIndex
            onCurrentIndexChanged: {
                styledItem.selectedIndex = currentIndex;
            }
            onCurrentItemChanged: {
                //adjust contentX so that the item is kept in the middle
                //don't use ListView.ApplyRange because that does an awkward animation when you select an item
                //*while* the current item is outside of screen
                positionItem(currentItem);
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
                activeFocusOnTab: false
                objectName: "section_button_" + index
                width: label.width + 2 * sectionsStyle.horizontalLabelSpacing
                height: sectionsStyle.height
                property bool selected: index === styledItem.selectedIndex
                onClicked: {
                    styledItem.selectedIndex = index;
                    sectionsListView.forceActiveFocus();
                }

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
                        baselineOffset: sectionsStyle.height < units.gu(4) ? -units.gu(1) : -units.gu(2)
                        horizontalCenter: parent.horizontalCenter
                    }

                    color: sectionButton.selected ?
                               sectionsStyle.selectedSectionColor :
                               sectionsStyle.sectionColor

                    Behavior on color {
                        ColorAnimation { duration: UbuntuAnimation.SlowDuration }
                    }
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
                objectName: "sections_scroll_animation"
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

        anchors.fill: parent
        hoverEnabled: true

        property bool pressedLeft: false
        property bool pressedRight: false
        onPressed: {
            pressedLeft = leftIcon.contains(mouse);
            pressedRight = rightIcon.contains(mouse);
            mouse.accepted = pressedLeft || pressedRight;
        }
        onClicked: {
            // positionViewAtIndex() does not provide animation
            var scrollDirection = 0;
            if (pressedLeft && leftIcon.contains(mouse)) {
                scrollDirection = -1;
            } else if (pressedRight && rightIcon.contains(mouse)) {
                scrollDirection = 1;
            } else {
                // User pressed on the left or right icon, and then released inside of the
                // MouseArea but outside of the icon that was pressed.
                return;
            }
            if (contentXAnim.running) contentXAnim.stop();
            var newContentX = sectionsListView.contentX + (sectionsListView.width * scrollDirection);
            contentXAnim.from = sectionsListView.contentX;
            // make sure we don't overshoot bounds
            contentXAnim.to = MathUtils.clamp(
                        newContentX,
                        0.0, // estimation of originX is some times wrong when scrolling towards the beginning
                        sectionsListView.originX + sectionsListView.contentWidth - sectionsListView.width);
            contentXAnim.start();
        }

        Icon {
            id: leftIcon
            objectName: "left_scroll_icon"
            // return whether the pressed event was done inside the area of the icon
            function contains(mouse) {
                return (mouse.x < listViewContainer.listViewMargins &&
                        !sectionsListView.atXBeginning);
            }
            anchors {
                left: parent.left
                leftMargin: (listViewContainer.listViewMargins - width) / 2
                bottom: parent.bottom
                bottomMargin: sectionsStyle.height < units.gu(4) ? units.gu(1) : units.gu(2)
            }
            width: units.gu(1)
            height: units.gu(1)
            visible: false
            rotation: 180
            opacity: visible
                     ? sectionsListView.atXBeginning ? hoveringArea.iconsDisabledOpacity : 1.0
                     : 0.0
            name: "chevron"
            Behavior on opacity {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.FastDuration
                }
            }
        }

        Icon {
            id: rightIcon
            objectName: "right_scroll_icon"
            // return whether the pressed event was done inside the area of the icon
            function contains(mouse) {
                return (mouse.x > (hoveringArea.width - listViewContainer.listViewMargins) &&
                        !sectionsListView.atXEnd);
            }
            anchors {
                right: parent.right
                rightMargin: (listViewContainer.listViewMargins - width) / 2
                bottom: parent.bottom
                bottomMargin: sectionsStyle.height < units.gu(4) ? units.gu(1) : units.gu(2)
            }
            width: units.gu(1)
            height: units.gu(1)
            visible: false
            opacity: visible
                     ? sectionsListView.atXEnd ? hoveringArea.iconsDisabledOpacity : 1.0
                     : 0.0
            name: "chevron"
            Behavior on opacity {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.FastDuration
                }
            }
        }
    }

    LinearGradient {
        id: gradient

        visible: false
        anchors.fill: parent
        start: Qt.point(0,0)
        end: Qt.point(width,0)

        property real gradientWidth: listViewContainer.listViewMargins / gradient.width
        //the width is gradientWidth, but we want the gradient to actually start/finish at gradientSplitPosition
        //just to leave some margin.
        property real gradientSplitPosition: 2/3 * gradientWidth

        gradient: Gradient {
            //left gradient
            GradientStop { position: 0.0 ; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: gradient.gradientSplitPosition ; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: gradient.gradientWidth; color: Qt.rgba(1,1,1,1) }
            //right gradient
            GradientStop { position: 1.0 - gradient.gradientWidth; color: Qt.rgba(1,1,1,1) }
            GradientStop { position: 1.0 - gradient.gradientSplitPosition; color: Qt.rgba(1,1,1,0) }
            GradientStop { position: 1.0; color: Qt.rgba(1,1,1,0) }
        }

            Rectangle {
                // full opacity for the underline at the bottom in the mask below
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: sectionsStyle.underlineHeight
                color: "white"
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
            PropertyChanges { target: leftIcon; visible: true; }
            PropertyChanges { target: rightIcon; visible: true; }
        }
    ]
}
