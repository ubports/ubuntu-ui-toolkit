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

    Connections {
        target: tabs
        onSelectedTabIndexChanged: {
            indicatorImage.visible = false;
            tabBar.active = false;
            buttonView.updateOffset();
        }
    }

    /*!
      Use the tab bar only to display the currently selected tab,
      or can the user interact with it and select a tab?
     */
    property bool active: false

    /*!
      \internal
      Avoid interpreting a click to activate the tab bar as a button click.
     */
    property bool __buttonsClickable: true
    Timer {
        id: buttonsClickableTimer
        interval: 300
        running: false
        onTriggered: __buttonsClickable = true
    }

    onActiveChanged: {
        if (active) {
            indicatorImage.visible = false;
            __buttonsClickable = false;
            buttonsClickableTimer.restart();
            deactivateTimer.restart();
        } else {
            buttonView.updateOffset();
        }
    }

    Timer {
        id: deactivateTimer
        interval: (itemStyle && itemStyle.deactivateTime) ?  itemStyle.deactivateTime : 1000
        running: true
        onTriggered: active = false
    }

    // used to position buttons and indicator image
    property real totalButtonWidth: 0
    property var relativeButtonPositions: [] // TODO: remove this property
    property var buttonOffsets: []

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width

            // set in onCompleted of the PathView
            property int rowNumber

            Component.onCompleted: {
                tabBar.totalButtonWidth = theRow.width;
                tabBar.relativeButtonPositions = [];
                tabBar.buttonOffsets = [];
                for (var i=0; i < children.length-1; i++) { // children[length-2] is the repeater
                    tabBar.relativeButtonPositions.push(children[i].x / tabBar.totalButtonWidth);
                    tabBar.buttonOffsets.push(1 - children[i].x / totalButtonWidth);
                }
                tabBar.buttonOffsets.push(0.0);
            }

            Repeater {
                id: repeater
                model: tabs.__tabsModel.children

                AbstractButton {
                    id: button
                    width: text.width + text.anchors.leftMargin + text.anchors.rightMargin
                    property bool selected: (index === tabs.selectedTabIndex)

                    anchors.top: parent.top
                    height: parent.height - itemStyle.headerTextBottomMargin

                    Label {
                        id: text
                        color: selected ? itemStyle.headerTextColorSelected : itemStyle.headerTextColor
                        opacity: (tabBar.active || selected) ? 1.0 : 0

                        Behavior on opacity {
                            NumberAnimation {
                                duration: 350
                            }
                        }

                        anchors {
                            left: parent.left
                            leftMargin: units.gu(2)
                            rightMargin: units.gu(0)
                            baseline: parent.bottom
                        }
                        text: title
                        fontSize: itemStyle.headerFontSize
                        font.weight: itemStyle.headerFontWeight
                        verticalAlignment: Text.AlignBottom
                    }
                    onClicked: {
                        if (tabBar.__buttonsClickable) {
                            buttonView.activeButtonRowNumber = theRow.rowNumber;
                            tabs.selectedTabIndex = index;
                            tabBar.active = false;
                        }
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
        delegate: tabButtonRow
        model: 2 // The second buttonRow shows the buttons that disappear on the left
        property bool needsScrolling: tabBar.totalButtonWidth > tabBar.width
        interactive: needsScrolling
        width: needsScrolling ? tabBar.width : tabBar.totalButtonWidth
        clip: !needsScrolling // avoid showing the same button twice

        highlightRangeMode: PathView.NoHighlightRange
        offset: 0
        path: Path {
            startX: -tabBar.totalButtonWidth/2
            PathLine {
                x: tabBar.totalButtonWidth*1.5
            }
        }

        // used to track (in combination with selectedTabIndex) which button instance was clicked
        property int activeButtonRowNumber: 1
        function updateOffset() {
            if (!tabBar.relativeButtonPositions) return;
            var newOffset = activeButtonRowNumber + tabBar.buttonOffsets[tabBar.tabs.selectedTabIndex];
            var diff = offset - newOffset;
            if (diff < -1) newOffset = newOffset - 2;
            offset = newOffset;
            indicatorImage.visible = true;
        }

        function buttonInRange(i) {
            var relativePosition = offset - Math.floor(offset);
            return (buttonOffsets[i] > relativePosition && relativePosition > buttonOffsets[i+1]);
        }

        Component.onCompleted: {
            children[1].rowNumber = 0;
            children[2].rowNumber = 1;
        }

        Behavior on offset {
            SmoothedAnimation {
                velocity: 1
            }
        }

        onMovementStarted: deactivateTimer.stop()
        onMovementEnded: deactivateTimer.restart()
    }

    Image {
        id: indicatorImage
        source: itemStyle.indicatorImageSource
        anchors {
            bottom: parent.bottom
            bottomMargin: itemStyle.headerTextBottomMargin
        }

        visible: !tabBar.active && !buttonView.moving
        opacity: visible ? 1 : 0

        Behavior on opacity {
            SequentialAnimation {
                PropertyAction { property: "opacity"; value: 0 }
                PauseAnimation { duration: 1000 }
                NumberAnimation { from: 0; to: 1; duration: 600 }
            }
        }

        x: getXPosition()

        function getXPosition() {
            var buttons = buttonView.children[1].children; // the first buttonRow
            var selectedButton = buttons[tabs.selectedTabIndex];
            return selectedButton.width + units.gu(2);
        }
    }

    MouseArea {
        // an inactive tabBar can be clicked to make it active
        id: mouseArea
        anchors.fill: parent
        enabled: !tabBar.active
        onPressed: {
            tabBar.active = true;
            mouse.accepted = false;
        }
    }
}
