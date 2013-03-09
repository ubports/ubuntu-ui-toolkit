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

Item {
    id: tabBar
    height: itemStyle.tabBarHeight

    /*!
      The set of tabs this tab bar belongs to
     */
    property Tabs tabs

    /*!
      An inactive tab bar only displays the currently selected tab,
      and an active tab bar can be interacted with to select a tab.
     */
    property bool active: false

    onActiveChanged: {
        if (active) {
            activatingTimer.restart();
        } else {
            buttonView.selectButton(tabs.selectedTabIndex);
        }
    }

    /*!
      \internal
      Avoid interpreting a click to activate the tab bar as a button click.
     */
    Timer {
        id: activatingTimer
        interval: 800 // same as pressAndHold time
    }

    Connections {
        target: tabs
        onSelectedTabIndexChanged: buttonView.selectButton(tabs.selectedTabIndex)
    }

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width
            property int rowNumber: modelData

            Repeater {
                id: repeater
                model: tabs.__tabsModel.children

                AbstractButton {
                    id: button
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: text.width + text.anchors.leftMargin + text.anchors.rightMargin

                    // When the tab bar is active, show both buttons corresponing to the tab index as selected,
                    // but when it is not active only one to avoid seeing fading animations of the unselected
                    // button when switching tabs from outside the tab bar.
                    property bool selected: (tabBar.active && buttonView.needsScrolling) ? tabs.selectedTabIndex === index : buttonView.selectedButtonIndex === button.buttonIndex
                    property real offset: theRow.rowNumber + 1 - button.x / theRow.width;
                    property int buttonIndex: index + theRow.rowNumber*repeater.count
                    Component.onCompleted: buttonView.buttons.push(button)

                    // Use opacity 0 to hide instead of setting visibility to false in order to
                    // make fading work well, and not to mess up width/offset computations
                    opacity: isVisible() ? selected ? itemStyle.headerTextSelectedOpacity : itemStyle.headerTextOpacity : 0
                    function isVisible() {
                        if (selected) return true;
                        if (!tabBar.active) return false;
                        if (buttonView.needsScrolling) return true;

                        // When we don't need scrolling, we want to avoid showing a button that is fading
                        // while sliding in from the right side when a new button was selected
                        var numTabs = tabs.__tabsModel.count;
                        var minimum = buttonView.selectedButtonIndex;
                        var maximum = buttonView.selectedButtonIndex + numTabs - 1;
                        if (MathUtils.clamp(buttonIndex, minimum, maximum) === buttonIndex) return true;
                        // working modulus numTabs:
                        if (buttonIndex < buttonView.selectedButtonIndex - numTabs) return true;
                        return false;
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: itemStyle.headerTextFadeDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Image {
                        id: indicatorImage
                        source: itemStyle.indicatorImageSource
                        anchors {
                            bottom: parent.bottom
                            bottomMargin: itemStyle.headerTextBottomMargin
                        }
                        x: button.width - width

                        // The indicator image must be visible after the selected tab button, when the
                        // tab bar is not active, or after the "last" button (starting with the selected one),
                        // when the tab bar is active.
                        property bool isLastAfterSelected: index === (tabs.selectedTabIndex === 0 ? repeater.count-1 : tabs.selectedTabIndex - 1)
                        opacity: (tabBar.active ? isLastAfterSelected : selected) ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: itemStyle.headerTextFadeDuration
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }

                    Label {
                        id: text
                        color: selected ? itemStyle.headerTextSelectedColor : itemStyle.headerTextColor

                        anchors {
                            left: parent.left
                            leftMargin: itemStyle.headerTextLeftMargin
                            rightMargin: itemStyle.headerTextRightMargin
                            baseline: parent.bottom
                            baselineOffset: -itemStyle.headerTextBottomMargin
                        }
                        text: modelData.title
                        fontSize: itemStyle.headerFontSize
                        font.weight: itemStyle.headerFontWeight
                    }

                    onClicked: {
                        if (!activatingTimer.running) {
                            tabs.selectedTabIndex = index;
                            tabBar.active = false;
                            button.select();
                        } else {
                            activatingTimer.stop();
                        }
                    }

                    // Select this button
                    function select() {
                        buttonView.selectedButtonIndex = button.buttonIndex;
                        buttonView.updateOffset();
                    }
                }
            }
        }
    }

    PathView {
        id: buttonView
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        // initial width needs to be parent.width, otherwise the contents will be messed up
        property real buttonRowWidth: currentItem ? currentItem.width : parent.width
        property var buttons: []

        // Track which button was last clicked
        property int selectedButtonIndex: 0

        delegate: tabButtonRow
        model: 2 // The second buttonRow shows the buttons that disappear on the left
        property bool needsScrolling: buttonRowWidth > tabBar.width
        interactive: needsScrolling
        width: needsScrolling ? tabBar.width : buttonRowWidth

        highlightRangeMode: PathView.NoHighlightRange
        offset: 0
        path: Path {
            startX: -buttonView.buttonRowWidth/2
            PathLine {
                x: buttonView.buttonRowWidth*1.5
            }
        }

        // x - y (mod a), for (x - y) <= a
        function cyclicDistance(x, y, a) {
            var r = x - y;
            return Math.min(Math.abs(r), Math.abs(r - a));
        }

        // Select the closest of the two buttons that represent the given tab index
        function selectButton(tabIndex) {
            var b1 = buttons[tabIndex];
            var b2 = buttons[tabIndex + tabs.__tabsModel.children.length];

            // find the button with the nearest offset
            var d1 = cyclicDistance(b1.offset, buttonView.offset, 2);
            var d2 = cyclicDistance(b2.offset, buttonView.offset, 2);
            if (d1 < d2) {
                b1.select();
            } else {
                b2.select();
            }
        }

        function updateOffset() {
            var newOffset = buttonView.buttons[buttonView.selectedButtonIndex].offset;
            if (offset - newOffset < -1) newOffset = newOffset - 2;
            offset = newOffset;
        }

        Behavior on offset {
            SmoothedAnimation {
                velocity: itemStyle.buttonPositioningVelocity
                easing.type: Easing.InOutQuad
            }
        }

        Component.onCompleted: {
            selectButton(tabs.selectedTabIndex)
        }

        onDragEnded: activatingTimer.stop()

        // deactivate the tab bar after inactivity
        onMovementStarted: idleTimer.stop()
        onMovementEnded: idleTimer.restart()
        Timer {
            id: idleTimer
            interval: (itemStyle && itemStyle.deactivateTime) ?  itemStyle.deactivateTime : 1000
            running: tabBar.active
            onTriggered: tabBar.active = false
        }
    }

    MouseArea {
        // an inactive tabBar can be pressed to make it active
        id: mouseArea
        anchors.fill: parent
        enabled: !tabBar.active
        onPressed: {
            tabBar.active = true;
            mouse.accepted = false;
        }
    }
}
