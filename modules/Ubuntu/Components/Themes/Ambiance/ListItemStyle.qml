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

    id: listItemStyle
    swipeOvershoot: units.gu(2)

    readonly property int listItemIndex: index

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

                Repeater {
                    model: itemActions.actions
                    AbstractButton {
                        id: actionButton
                        action: modelData
                        enabled: action.enabled
                        opacity: action.enabled ? 1.0 : 0.5
                        width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, actionsRow.maxItemWidth)
                        anchors {
                            top: parent ? parent.top : undefined
                            bottom: parent ? parent.bottom : undefined
                        }
                        function trigger() {
                            internals.selectedAction = modelData;
                            listItemStyle.rebound();
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

    Object {
        id: internals
        // action triggered
        property Action selectedAction
        // swipe handling
        readonly property Item swipedPanel: listItemStyle.x > 0 ? leadingLoader.item : trailingLoader.item
        readonly property bool leadingPanel: listItemStyle.x > 0
        readonly property real swipedOffset: swipedPanel ? swipedPanel.swipedOffset : 0
        readonly property real panelWidth: swipedPanel ? swipedPanel.panelWidth : 0
        property real prevX: 0.0
        property real snapChangerLimit: 0.0
        property real threshold: units.gu(1)
        property bool snapIn: false

        function updateSwipe() {
            if (prevX < listItemStyle.x && (snapChangerLimit <= listItemStyle.x)) {
                snapIn = leadingPanel;
                snapChangerLimit = listItemStyle.x - threshold;
            } else if (prevX > listItemStyle.x && (listItemStyle.x < snapChangerLimit)) {
                snapIn = !leadingPanel;
                snapChangerLimit = listItemStyle.x + threshold;
            }
            prevX = listItemStyle.x;
        }
        function snap() {
            var snapPos = (swipedOffset > units.gu(2) && snapIn) ? panelWidth : 0.0;
            snapAnimation.snapTo(snapPos);
        }
    }
    UbuntuNumberAnimation {
        id: snapAnimation
        target: styledItem.contentItem
        property: "x"
        duration: UbuntuAnimation.SnapDuration
        onStopped: {
            if (to == styledItem.contentItem.anchors.leftMargin && internals.selectedAction) {
                internals.selectedAction.trigger(listItemIndex);
                internals.selectedAction = null;
            }
        }
        function snapTo(pos) {
            if (pos == to && from == to) {
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

    onXChanged: internals.updateSwipe()
    onSwipeEvent: {
        if (event.status == SwipeEvent.Start) {
            internals.prevX = x;
            snapAnimation.stop();
        } else if (event.status == SwipeEvent.Stop) {
            internals.snap();
        }
    }
    onRebound: snapAnimation.snapTo(0)
}
