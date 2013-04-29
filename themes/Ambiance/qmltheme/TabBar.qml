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
    // styling properties
    property color headerTextColor
    property color headerTextSelectedColor
    property real headerTextOpacity
    property real headerTextSelectedOpacity
    property int headerTextFadeDuration
    property url indicatorImageSource

    property string headerFontSize
    property int headerFontWeight
    property real headerTextLeftMargin
    property real headerTextRightMargin
    property real headerTextBottomMargin

    property real buttonPositioningVelocity
    property int deactivateTime: 1000
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
//            buttonView.selectButton(tabs.selectedTabIndex);
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

//    Connections {
//        target: tabs
//        onSelectedTabIndexChanged: buttonView.selectButton(tabs.selectedTabIndex)
//    }

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width

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
                    property bool selected: false

                    Label {
                        id: text
                        color: selected ? headerTextSelectedColor : headerTextColor

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
                        selected = true;
                        tabs.selectedTabIndex = index;
//                        if (!activatingTimer.running) {
//                            tabs.selectedTabIndex = index;
//                            tabBar.active = false;
//                            button.select();
//                        } else {
//                            activatingTimer.stop();
//                        }
                    }

                    // Select this button
//                    function select() {
//                        buttonView.selectedButtonIndex = button.buttonIndex;
//                        buttonView.updateOffset();
//                    }
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

        delegate: tabButtonRow
        model: 2 // The second buttonRow shows the buttons that disappear on the left
//        property bool needsScrolling: buttonRowWidth > tabBar.width
        interactive: true //needsScrolling
//        width: needsScrolling ? tabBar.width : buttonRowWidth
//        anchors.right: parent.right
//        width: 799
        Binding {
            target: buttonView
            property: "rowWidth"
            value: buttonView.currentItem ? buttonView.currentItem.width : 555
        }

        currentIndex: 1
        property real rowWidth //: currentItem ? currentItem.width : width
        onCurrentItemChanged: print("current item = "+currentItem)
        onRowWidthChanged: print("rowWidth: "+rowWidth)

//        highlightRangeMode: PathView.NoHighlightRange
//        offset: 0
        path: Path {
//            startX: -buttonView.buttonRowWidth/2
//            PathLine {
//                x: buttonView.buttonRowWidth*1.5
//            }
            startX: 0
            PathLine {
                x: buttonView.rowWidth
            }
        }

//        // x - y (mod a), for (x - y) <= a
//        function cyclicDistance(x, y, a) {
//            var r = x - y;
//            return Math.min(Math.abs(r), Math.abs(r - a));
//        }

//        // Select the closest of the two buttons that represent the given tab index
//        function selectButton(tabIndex) {
//            if (tabIndex < 0 || tabIndex >= tabs.__tabs.length) return;
//            var b1 = buttons[tabIndex];
//            var b2 = buttons[tabIndex + tabs.__tabs.length];

//            // find the button with the nearest offset
//            var d1 = cyclicDistance(b1.offset, buttonView.offset, 2);
//            var d2 = cyclicDistance(b2.offset, buttonView.offset, 2);
//            if (d1 < d2) {
//                b1.select();
//            } else {
//                b2.select();
//            }
//        }

//        function updateOffset() {
//            var newOffset = buttonView.buttons[buttonView.selectedButtonIndex].offset;
//            if (offset - newOffset < -1) newOffset = newOffset - 2;
//            offset = newOffset;
//        }

//        Behavior on offset {
//            SmoothedAnimation {
//                velocity: buttonPositioningVelocity
//                easing.type: Easing.InOutQuad
//            }
//        }

//        Component.onCompleted: {
//            selectButton(tabs.selectedTabIndex);
//            buttonRowWidth = currentItem.width;
//        }

//        onDragEnded: activatingTimer.stop()

//        // deactivate the tab bar after inactivity
//        onMovementStarted: idleTimer.stop()
//        onMovementEnded: idleTimer.restart()
//        Timer {
//            id: idleTimer
//            interval: tabBar.deactivateTime
//            running: tabBar.active
//            onTriggered: tabBar.active = false
//        }
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
