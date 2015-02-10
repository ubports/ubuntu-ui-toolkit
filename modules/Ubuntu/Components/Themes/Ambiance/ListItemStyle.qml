/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.3
import Ubuntu.Components.Styles 1.2 as Styles
import Ubuntu.Components 1.2

Styles.ListItemStyle {

    id: style
    swipeOvershoot: units.gu(2)

    // anchoring
    anchors {
        top: parent ? parent.top : undefined
        bottom: parent ? parent.bottom : undefined
        bottomMargin: styledItem.divider.visible ? styledItem.divider.height : 0
        left: styledItem.contentItem.left
        leftMargin: -styledItem.contentItem.anchors.leftMargin
        right: styledItem.contentItem.right
        rightMargin: -styledItem.contentItem.anchors.rightMargin
    }

    // leading/trailing panels
    Component {
        id: panelComponent
        Rectangle {
            id: panel
            property bool leading: false
            property real paintedActionWidth: units.gu(2.5)
            property color foregroundColor: leading ? "white" : UbuntuColors.darkGrey
            readonly property real panelWidth: actionsRow.width
            readonly property real swipedOffset: leading ? panelWidth + x : styledItem.width - x;

            color: leading ? UbuntuColors.red : "white"
            anchors.fill: parent
            width: parent.width

            readonly property ListItemActions itemActions: leading ? styledItem.leadingActions : styledItem.trailingActions

            Row {
                id: actionsRow
                anchors {
                    left: leading ? undefined : parent.left
                    right: leading ? parent.right : undefined
                    top: parent.top
                    bottom: parent.bottom
                    leftMargin: spacing
                }

                property real maxItemWidth: parent.width / itemActions.actions.length

                property Action selectedAction
                property int listItemIndex: -1

                Repeater {
                    model: itemActions.actions
                    AbstractButton {
                        id: actionButton
                        action: modelData
                        enabled: action.enabled
                        opacity: action.enabled ? 1.0 : 0.5
                        width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, actionsRow.maxItemWidth)
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                        function trigger() {
                            actionsRow.selectedAction = modelData;
                            actionsRow.listItemIndex = styledItem.index;
                            snapAnimation.snapToPosition(0.0);
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: Theme.palette.selected.background
                            visible: pressed
                        }

                        Loader {
                            id: delegateLoader
                            height: parent.height
                            sourceComponent: itemActions.delegate ? itemActions.delegate : defaultDelegate
                            property Action action: modelData
                            property int index: index
                            property bool pressed: actionButton.pressed
                            onItemChanged: {
                                // use action's objectName to identify the visualized action
                                if (item && item.objectName === "") {
                                    item.objectName = modelData.objectName;
                                    actionButton.objectName = "actionbutton_" + modelData.objectName
                                }
                            }
                        }
                    }
                }
            }

            Component {
                id: defaultDelegate
                Item {
                    width: height
                    Icon {
                        width: panel.paintedActionWidth
                        height: width
                        name: action.iconName
                        color: panel.foregroundColor
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }

    Loader {
        id: leadingLoader
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.left
        }
        width: parent.width
        sourceComponent: styledItem.leadingActions && styledItem.leadingActions.actions.length > 0 ?
                             panelComponent : null
        onItemChanged: {
            if (item) {
                item.leading = true;
            }
        }
    }
    Loader {
        id: trailingLoader
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.right
        }
        width: parent.width
        sourceComponent: styledItem.trailingActions && styledItem.trailingActions.actions.length > 0 ?
                             panelComponent : null
        onItemChanged: {
            if (item) {
                item.leading = false;
            }
        }
    }

    // swipe handling
    readonly property Item swipedPanel: x > 0 ? leadingLoader.item : trailingLoader.item
    readonly property bool leadingPanel: x > 0
    property real prevX: 0.0
    property real snapChangerLimit: 0.0
    property real threshold: units.gu(1)
    property bool snapIn: false
    UbuntuNumberAnimation {
        id: snapAnimation
        target: styledItem.contentItem
        property: "x"
        duration: UbuntuAnimation.SnapDuration

        function snapToPosition(pos) {
            if (pos == to) {
                return;
            }

            stop();
            from = styledItem.contentItem.x;
            if (!pos) {
                pos = styledItem.contentItem.anchors.leftMargin;
            }
            to = pos;
            start();
            print("snapTo", to)
        }
    }
    onXChanged: {
        if (prevX < x && (snapChangerLimit <= x)) {
            snapIn = leading;
            snapChangerLimit = x - threshold;
        } else if (prevX > x && (x < snapChangerLimit)) {
            snapIn = !leading;
            snapChangerLimit = x + threshold;
        }
        prevX = x;
    }
    onSwipeEvent: {
        if (event.status == SwipeEvent.Start) {
            snapAnimation.stop();
        } else if (event.status == SwipeEvent.Stop) {
            var swipedOffset = swipedPanel ? swipedPanel.swipedOffset : 0
            var panelWidth = swipedPanel ? swipedPanel.panelWidth : 0
            var snapPos = (swipedOffset > units.gu(2) && snapIn) ? panelWidth : 0.0;
            snapAnimation.snapToPosition(snapPos);
        }
    }
    onRebound: snapAnimation.snapToPosition(0)
}
