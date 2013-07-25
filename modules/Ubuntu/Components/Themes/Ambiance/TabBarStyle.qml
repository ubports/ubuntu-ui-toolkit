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
    id: tabBarStyle
    // styling properties
    property color headerTextColor: Theme.palette.normal.backgroundText
    property color headerTextSelectedColor: Theme.palette.selected.backgroundText

    property int headerTextFadeDuration: 350
    property url indicatorImageSource: "artwork/chevron.png"

    property string headerFontSize: "x-large"
    property int headerFontWeight: Font.Light
    property real headerTextLeftMargin: units.gu(2)
    property real headerTextRightMargin: units.gu(2)
    property real headerTextBottomMargin: units.gu(2)

    property real buttonPositioningVelocity: 1.0
    property int deactivateTime: 3000

    /*!
      The set of tabs this tab bar belongs to
     */
    property Tabs tabs: styledItem ? styledItem.tabsItem : null

    Connections {
        target: styledItem

        onActiveChanged: {
            if (styledItem.active) {
                activatingTimer.restart();
            } else {
                buttonView.selectButton(tabs.selectedTabIndex);
            }
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
        onModelChanged: buttonView.selectButton(tabs.selectedTabIndex)
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

            Component.onCompleted: {
                if (rowNumber === 0) {
                    buttonView.buttonRow1 = theRow;
                } else {
                    buttonView.buttonRow2 = theRow;
                }
            }

            Repeater {
                id: repeater
                model: tabs.__tabs

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
                    property bool selected: (styledItem.active && buttonView.needsScrolling) ? tabs.selectedTabIndex === index : buttonView.selectedButtonIndex === button.buttonIndex
                    property real offset: theRow.rowNumber + 1 - button.x / theRow.width;
                    property int buttonIndex: index + theRow.rowNumber*repeater.count

                    // Use opacity 0 to hide instead of setting visibility to false in order to
                    // make fading work well, and not to mess up width/offset computations
                    opacity: isVisible() ? 1.0 : 0.0
                    function isVisible() {
                        if (selected) return true;
                        if (!styledItem.active) return false;
                        if (buttonView.needsScrolling) return true;

                        // When we don't need scrolling, we want to avoid showing a button that is fading
                        // while sliding in from the right side when a new button was selected
                        var numTabs = tabs.__tabs.length;
                        var minimum = buttonView.selectedButtonIndex;
                        var maximum = buttonView.selectedButtonIndex + numTabs - 1;
                        if (MathUtils.clamp(buttonIndex, minimum, maximum) === buttonIndex) return true;
                        // working modulus numTabs:
                        if (buttonIndex < buttonView.selectedButtonIndex - numTabs) return true;
                        return false;
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: headerTextFadeDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Image {
                        id: indicatorImage
                        source: indicatorImageSource
                        anchors {
                            bottom: parent.bottom
                            bottomMargin: headerTextBottomMargin
                        }
                        x: button.width - width

                        // The indicator image must be visible after the selected tab button, when the
                        // tab bar is not active, or after the "last" button (starting with the selected one),
                        // when the tab bar is active.
                        property bool isLastAfterSelected: index === (tabs.selectedTabIndex === 0 ? repeater.count-1 : tabs.selectedTabIndex - 1)
                        opacity: (styledItem.active ? isLastAfterSelected : selected) ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: headerTextFadeDuration
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }

                    Label {
                        id: text
                        color: selected ? headerTextSelectedColor : headerTextColor

                        Behavior on color {
                            ColorAnimation {
                                duration: headerTextFadeDuration
                                easing.type: Easing.InOutQuad
                            }
                        }

                        anchors {
                            left: parent.left
                            leftMargin: headerTextLeftMargin
                            rightMargin: headerTextRightMargin
                            baseline: parent.bottom
                            baselineOffset: -headerTextBottomMargin
                        }
                        text: modelData.title
                        fontSize: headerFontSize
                        font.weight: headerFontWeight
                    }

                    onClicked: {
                        if (!activatingTimer.running) {
                            tabs.selectedTabIndex = index;
                            if (!styledItem.alwaysActive) {
                                styledItem.active = false;
                            }
                            button.select();
                        } else {
                            activatingTimer.stop();
                        }
                    }

                    // Select this button
                    function select() {
                        buttonView.selectedButtonIndex = button.buttonIndex;
                        buttonView.updateOffset(button.offset);
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

        // set to the width of one tabButtonRow in Component.onCompleted.
        property real buttonRowWidth: buttonRow1 ? buttonRow1.width : 0

        // set by the delegate when the components are completed.
        property Row buttonRow1
        property Row buttonRow2

        // Track which button was last clicked
        property int selectedButtonIndex: -1

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
            if (tabIndex < 0 || tabIndex >= tabs.__tabs.length) return;
            if (buttonView.buttonRow1 && buttonView.buttonRow2) {
                var b1 = buttonView.buttonRow1.children[tabIndex];
                var b2 = buttonView.buttonRow2.children[tabIndex];

                // find the button with the nearest offset
                var d1 = cyclicDistance(b1.offset, buttonView.offset, 2);
                var d2 = cyclicDistance(b2.offset, buttonView.offset, 2);
                if (d1 < d2) {
                    b1.select();
                } else {
                    b2.select();
                }
            }
        }

        function updateOffset(newOffset) {
            if (offset - newOffset < -1) newOffset = newOffset - 2;
            offset = newOffset;
        }

        Behavior on offset {
            SmoothedAnimation {
                velocity: buttonPositioningVelocity
                easing.type: Easing.InOutQuad
            }
        }

        Component.onCompleted: {
            selectButton(tabs.selectedTabIndex);
        }

        onDragEnded: activatingTimer.stop()

        // deactivate the tab bar after inactivity
        onMovementStarted: idleTimer.stop()
        onMovementEnded: {
            if (!styledItem.alwaysActive) {
                idleTimer.restart();
            }
        }
        Timer {
            id: idleTimer
            interval: tabBarStyle.deactivateTime
            running: styledItem.active && !styledItem.alwaysActive
            onTriggered: styledItem.active = false
        }
    }

    MouseArea {
        // an inactive tabBar can be pressed to make it active
        id: mouseArea
        anchors.fill: parent
        enabled: !styledItem.active
        onPressed: {
            styledItem.active = true;
            mouse.accepted = false;
        }
    }
}
