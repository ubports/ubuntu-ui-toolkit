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

Rectangle {
    id: control
    // style properties
    property color frameColor: UbuntuColors.warmGrey
    property real frameWidth: 1
    property color backgroundColor: "#00000000"
    property bool overlay: true
    property color overlayColor: UbuntuColors.warmGrey

    // private properties
    property bool completed: false

    border {
        width: frameWidth
        color: frameColor
    }
    color: backgroundColor
    anchors.fill: parent

    function modelSize() {
        return loader.item.model.hasOwnProperty("count") ? loader.item.model.count : loader.item.model.length;
    }

    function moveToIndex(toIndex) {
        var count = (loader.item && loader.item.model) ? modelSize() : -1;
        if (completed && count > 0) {
            if (QuickUtils.className(loader.item) === "QQuickListView") {
                loader.item.currentIndex = toIndex;
                return;
            } else {
                loader.item.positionViewAtIndex(count - 1, PathView.Center);
                loader.item.positionViewAtIndex(toIndex, PathView.Center);
            }
        }
    }

    Component {
        id: highlightComponent
        Item {
            width: parent ? parent.width : 0
            height: (parent && parent.currentItem) ? parent.currentItem.height : units.gu(4);
        }
    }

    // circular list
    Component {
        id: wrapAround
        PathView {
            id: pView
            property Item tumbler: styledItem
            anchors.fill: parent
            clip: true

            model: styledItem.model
            delegate: styledItem.delegate
            highlight: highlightComponent
            // put the currentItem to the center of the view
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5

            pathItemCount: pView.height / highlightItem.height + 1
            snapMode: PathView.SnapToItem
            flickDeceleration: 100

            property int contentHeight: pathItemCount * highlightItem.height
            path: Path {
                startX: pView.width / 2
                startY: -(pView.contentHeight - pView.height) / 2
                PathLine {
                    x: pView.width / 2
                    y: pView.height + (pView.contentHeight - pView.height) / 2
                }
            }
        }
    }

    // linear list
    Component {
        id: linear
        ListView {
            id: lView
            property Item tumbler: styledItem

            anchors.fill: parent
            clip: true

            model: styledItem.model
            delegate: styledItem.delegate

            highlight: highlightComponent
            preferredHighlightBegin: highlightItem ? (height - highlightItem.height) / 2 : 0
            preferredHighlightEnd: highlightItem ? (preferredHighlightBegin + highlightItem.height) : 0
            highlightRangeMode: ListView.StrictlyEnforceRange
            highlightMoveDuration: 300
            flickDeceleration: 100
        }
    }

    Loader {
        id: loader
        asynchronous: false
        anchors {
            fill: parent
            margins: units.gu(0.2)
        }
        sourceComponent: (styledItem.circular) ? wrapAround : linear

        // to avoid binding loop
        Connections {
            target: styledItem
            onSelectedIndexChanged: loader.item.currentIndex = styledItem.selectedIndex
        }

        // live selectedIndex updater
        Binding {
            target: styledItem
            property: "selectedIndex"
            value: loader.item.currentIndex
            when: completed && (styledItem.model !== undefined) && styledItem.live
        }
        // non-live selectedIndex updater
        Connections {
            target: loader.item
            ignoreUnknownSignals: true
            onMovementEnded: {
                if (!styledItem.live) styledItem.selectedIndex = loader.item.currentIndex;
            }
            onCurrentIndexChanged: {
                if (!styledItem.live && styledItem.__clickedIndex === loader.item.currentIndex) {
                    styledItem.selectedIndex = loader.item.currentIndex;
                    styledItem.__clickedIndex = -1;
                }
            }
            onModelChanged: {
                moveToIndex((completed) ? 0 : styledItem.selectedIndex);
                if (completed && !styledItem.live) styledItem.selectedIndex = 0;
            }
        }

        Component.onCompleted: {
            completed = true;
            if (item) {
                loader.item.currentIndex = styledItem.selectedIndex;
                moveToIndex(styledItem.selectedIndex);
            }
        }

        // overlay
        Rectangle {
            visible: control.overlay
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: control.overlayColor
                }

                GradientStop {
                    position: 0.40
                    color: "#ffffff"
                }

                GradientStop {
                    position: 0.63
                    color: "#ffffff"
                }

                GradientStop {
                    position: 1
                    color: control.overlayColor
                }
            }
            anchors.fill: parent
        }
    }

}
