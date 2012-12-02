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
    height: units.gu(6.5)

    property Tabs tabs

    Connections {
        target: tabs
        onSelectedTabIndexChanged: {
            tabBar.active = false;
            buttonView.position()
        }
    }

    property bool active: false
    onActiveChanged: {
        if (!active) buttonView.position();
    }
    Component.onCompleted: buttonView.position();

    // used to position buttons and indicator image
    property real totalButtonWidth: 0
    property var relativeButtonPositions: []

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width

            Component.onCompleted: {
                tabBar.totalButtonWidth = theRow.width;
                tabBar.relativeButtonPositions = [];
                for (var i=0; i < children.length-1; i++) { // children[length-2] is the repeater
                    tabBar.relativeButtonPositions.push(children[i].x / tabBar.totalButtonWidth);
                }
            }

            Repeater {
                id: repeater

                model: tabs.__tabsModel.children

                AbstractButton {
                    id: button
                    width: text.width + text.anchors.leftMargin + text.anchors.rightMargin
                    property bool selected: (index === tabs.selectedTabIndex)

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    Label {
                        id: text
                        color: selected ? itemStyle.headerTextColorSelected : itemStyle.headerTextColor
                        visible: tabBar.active || selected
                        anchors {
                            left: parent.left
                            leftMargin: units.gu(2)
                            rightMargin: units.gu(0)
                            bottom: parent.bottom
                            bottomMargin: units.gu(1)
                        }
                        text: title
                        fontSize: itemStyle.headerFontSize
                        verticalAlignment: Text.AlignBottom
                    }

                    onClicked: {
                        tabs.selectedTabIndex = index;
                        tabBar.active = false;
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
        interactive: (tabBar.totalButtonWidth > tabBar.width)
        width: Math.min(tabBar.width, tabBar.totalButtonWidth)
        clip: true // avoid showing the same button twice

        model: 2
        delegate: tabButtonRow
        highlightRangeMode: PathView.NoHighlightRange
        offset: (model === 1) ? 0.5 : 0
        path: Path {
            startX: -tabBar.totalButtonWidth/2
            PathLine {
                x: tabBar.totalButtonWidth*1.5
            }
        }
        function position() {
            if (!tabBar.relativeButtonPositions) return;
            offset = 1 - tabBar.relativeButtonPositions[tabBar.tabs.selectedTabIndex];
        }
    }

    Image {
        id: indicatorImage
        source: itemStyle.indicatorImageSource
        anchors {
            bottom: parent.bottom
            bottomMargin: units.gu(2)
        }

        visible: !tabBar.active

        x: getXPosition()

        function getXPosition() {
            var buttons = buttonView.children[1].children; // the first buttonRow
            var selectedButton = buttons[tabs.selectedTabIndex];
            return selectedButton.width + units.gu(2);
        }
    }

    MouseArea {
        // an inactive tabBar can be clicked to make it active
        anchors.fill: parent
        enabled: !tabBar.active
        onClicked: {
            tabBar.active = true;
        }
    }
}
