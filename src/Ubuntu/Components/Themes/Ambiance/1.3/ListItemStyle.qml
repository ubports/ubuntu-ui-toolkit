/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components.Styles 1.3 as Styles
import Ubuntu.Components 1.3

Styles.ListItemStyle {

    id: listItemStyle

    /*
     * Coloring properties
     */
    property color leadingPanelColor: theme.palette.normal.foreground
    property color trailingPanelColor: theme.palette.normal.foreground
    property color leadingForegroundColor: theme.palette.normal.negative
    property color trailingForegroundColor: theme.palette.normal.foregroundText

    // anchoring
    anchors {
        // do not anchor fill
        fill: undefined
        top: parent ? parent.top : undefined
        bottom: parent ? parent.bottom : undefined
        bottomMargin: styledItem.divider.visible ? styledItem.divider.height : 0
        left: styledItem.contentItem.left
        leftMargin: -styledItem.contentItem.anchors.leftMargin
        right: styledItem.contentItem.right
        rightMargin: -styledItem.contentItem.anchors.rightMargin
    }
    LayoutMirroring.childrenInherit: true

    // leading/trailing panels
    Component {
        id: panelComponent
        Rectangle {
            id: panel
            objectName: "ListItemPanel" + (leading ? "Leading" : "Trailing")
            // add 0.5 GUs to the panel size so we get 2GU default margin on the first action
            readonly property real panelWidth: actionsRow.width + units.gu(0.5)

            color: leading ? leadingPanelColor : trailingPanelColor
            anchors.fill: parent

            readonly property ListItemActions itemActions: leading ? styledItem.leadingActions : styledItem.trailingActions

            Row {
                id: actionsRow
                anchors {
                    left: leading ? undefined : parent.left
                    right: leading ? parent.right : undefined
                    leftMargin: leading ? 0 : units.gu(0.5)
                    rightMargin: leading ? units.gu(0.5) : 0
                    top: parent.top
                    bottom: parent.bottom
                }

                readonly property real maxItemWidth: parent.width / itemActions.actions.length
                readonly property real minItemWidth: units.gu(6) // 2GU icon + 2* 2GU margin

                Repeater {
                    model: itemActions.actions
                    AbstractButton {
                        id: actionButton
                        action: modelData
                        enabled: action.enabled
                        opacity: action.enabled ? 1.0 : 0.5
                        width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, actionsRow.minItemWidth, actionsRow.maxItemWidth)
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
                            color: theme.palette.selected.background
                            visible: pressed
                        }

                        Loader {
                            id: delegateLoader
                            height: parent.height
                            sourceComponent: itemActions.delegate ? itemActions.delegate : defaultDelegate
                            property Action action: modelData
                            property int index: listItemIndex
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
                    width: actionsRow.minItemWidth
                    Icon {
                        width: units.gu(2)
                        height: width
                        name: action.iconName
                        color: leading ? leadingForegroundColor : trailingForegroundColor
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
    // the selection/multiselection panel
    Component {
        id: selectionDelegate
        Item {
            id: selectPanel
            objectName: "selection_panel" + listItemIndex
            anchors.fill: parent ? parent : undefined

            CheckBox {
                id: checkbox
                opacity: 0
                // for unit and autopilot tests
                objectName: "listitem_select"
                anchors.centerIn: parent
                // for the initial value
                checked: styledItem.selected
                onCheckedChanged: styledItem.selected = checked;
                Binding {
                    target: checkbox
                    property: "checked"
                    value: styledItem.selected
                }
            }

            states: State {
                name: "enabled"
                when: loaded && styledItem.selectMode
                PropertyChanges {
                    target: checkbox
                    opacity: 1.0
                }
            }
            transitions: Transition {
                from: ""
                to: "*"
                reversible: true
                enabled: listItemStyle.animatePanels
                OpacityAnimator {
                    easing: UbuntuAnimation.StandardEasing
                    duration: UbuntuAnimation.FastDuration
                }
            }
        }
    }
    // drag panel
    Component {
        id: dragDelegate
        Item {
            id: dragPanel
            objectName: "drag_panel" + listItemIndex
            anchors.fill: parent ? parent : undefined
            Icon {
                objectName: "icon"
                id: dragIcon
                anchors.centerIn: parent
                width: units.gu(3)
                height: width
                name: "view-grid-symbolic"
                opacity: 0.0
                scale: 0.5
                color: theme.palette.normal.base
            }
            Binding {
                target: listItemStyle
                property: "dragPanel"
                value: dragPanel
            }

            states: State {
                name: "enabled"
                when: loaded && styledItem.dragMode
                PropertyChanges {
                    target: dragIcon
                    opacity: 1.0
                    scale: 1.0
                }
            }
            transitions: Transition {
                from: ""
                to: "*"
                reversible: true
                enabled: listItemStyle.animatePanels
                ParallelAnimation {
                    OpacityAnimator {
                        easing: UbuntuAnimation.StandardEasing
                        duration: UbuntuAnimation.FastDuration
                    }
                    ScaleAnimator {
                        easing: UbuntuAnimation.StandardEasing
                        duration: UbuntuAnimation.FastDuration
                    }
                }
            }
        }
    }

    // leading panel loader
    Loader {
        id: leadingLoader
        objectName: "leading_loader"
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.left
        }
        width: styledItem.width
        sourceComponent: styledItem.swiped && styledItem.leadingActions && styledItem.leadingActions.actions.length > 0 ?
                             panelComponent : null
        // context properties used in delegates
        readonly property bool leading: true
        readonly property bool loaded: status == Loader.Ready

        // panel states
        states: [
            State {
                name: "selectable"
                when: styledItem.selectMode
                PropertyChanges {
                    target: leadingLoader
                    sourceComponent: selectionDelegate
                    width: units.gu(5)
                }
                PropertyChanges {
                    target: listItemStyle
                    anchors.leftMargin: 0
                }
                PropertyChanges {
                    target: styledItem.contentItem
                    anchors.leftMargin: units.gu(5)
                }
            }
        ]
        transitions: Transition {
            from: ""
            to: "selectable"
            reversible: true
            enabled: listItemStyle.animatePanels
            PropertyAnimation {
                target: styledItem.contentItem
                properties: "anchors.leftMargin"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }
    }
    // trailing panel loader
    Loader {
        id: trailingLoader
        objectName: "trailing_loader"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.right
        }
        width: styledItem.width
        sourceComponent: styledItem.swiped && styledItem.trailingActions && styledItem.trailingActions.actions.length > 0 ?
                             panelComponent : null
        // context properties used in delegates
        readonly property bool leading: false
        readonly property bool loaded: status == Loader.Ready

        // panel states
        states: State {
            name: "draggable"
            when: styledItem.dragMode
            PropertyChanges {
                target: trailingLoader
                sourceComponent: dragDelegate
                width: units.gu(5)
            }
            PropertyChanges {
                target: listItemStyle
                anchors.rightMargin: 0
            }
            PropertyChanges {
                target: styledItem.contentItem
                anchors.rightMargin: units.gu(5)
            }
        }
        transitions: Transition {
            from: ""
            to: "*"
            reversible: true
            enabled: listItemStyle.animatePanels
            PropertyAnimation {
                target: styledItem.contentItem
                properties: "anchors.rightMargin"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }
    }

    // internals
    QtObject {
        id: internals
        // action triggered
        property Action selectedAction
        // swipe handling
        readonly property Item swipedPanel: leadingPanel ? leadingLoader.item : trailingLoader.item
        readonly property bool leadingPanel: listItemStyle.LayoutMirroring.enabled ? (listItemStyle.x < 0) : (listItemStyle.x > 0)
        readonly property real swipedOffset: (leadingPanel ? listItemStyle.x : -listItemStyle.x) *
                                             (listItemStyle.LayoutMirroring.enabled ? -1 : 1)
        readonly property real panelWidth: swipedPanel && swipedPanel.hasOwnProperty("panelWidth") ? swipedPanel.panelWidth : 0
        property real prevX: 0.0
        property real snapChangerLimit: 0.0
        readonly property real threshold: units.gu(1.5)
        property bool snapIn: false
        property bool completed: false

        // update snap direction
        function updateSnapDirection() {
            if (prevX < listItemStyle.x && (snapChangerLimit <= listItemStyle.x)) {
                snapIn = (listItemStyle.LayoutMirroring.enabled !== leadingPanel);
                snapChangerLimit = listItemStyle.x - threshold;
            } else if (prevX > listItemStyle.x && (listItemStyle.x < snapChangerLimit)) {
                snapIn = (listItemStyle.LayoutMirroring.enabled === leadingPanel);
                snapChangerLimit = listItemStyle.x + threshold;
            }
            prevX = listItemStyle.x;
        }
        // perform snapIn/Out
        function snap() {
            var snapPos = (swipedOffset > units.gu(2) && snapIn) ? panelWidth : 0.0;
            snapPos *= leadingPanel ? 1 : -1;
            // invert snapPos on RTL
            snapPos *= listItemStyle.LayoutMirroring.enabled ? -1 : 1;
            snapAnimation.snapTo(snapPos);
        }
        // handle elasticity on overshoot
        function overshoot(event) {
            var offset = event.content.x - styledItem.contentItem.anchors.leftMargin;
            offset *= leadingPanel ? 1 : -1;
            // invert offset on RTL
            offset *= listItemStyle.LayoutMirroring.enabled ? -1 : 1;
            if (offset > panelWidth) {
                // do elastic move
                event.content.x = styledItem.contentItem.x + (event.to.x - event.from.x) / 2;
            }
        }
    }
    snapAnimation: SmoothedAnimation {
        objectName: "snap_animation"
        target: styledItem.contentItem
        property: "x"
        // use 50GU/second velocity
        velocity: units.gu(60)
        onStopped: {
            // trigger action
            if (to == styledItem.contentItem.anchors.leftMargin && internals.selectedAction) {
                internals.selectedAction.trigger(listItemIndex);
                internals.selectedAction = null;
            }
        }
        // animated snapping
        function snapTo(pos) {
            if (pos == to && styledItem.contentItem.x == to) {
                return;
            }

            stop();
            from = styledItem.contentItem.x;
            // add leading margin as the snapped out item's X coordinate is the leftMargin
            pos += styledItem.contentItem.anchors.leftMargin;
            to = pos;
            start();
        }
    }

    // simple drop animation
    dropAnimation: SmoothedAnimation {
        properties: "y"
        velocity: units.gu(60)
    }

    onXChanged: internals.updateSnapDirection()
    // overriding default functions
    function swipeEvent(event) {
        if (event.status == SwipeEvent.Started) {
            internals.prevX = x;
            snapAnimation.stop();
        } else if (event.status == SwipeEvent.Finished) {
            internals.snap();
        } else if (event.status == SwipeEvent.Updated) {
            // handle elasticity when overshooting
            internals.overshoot(event)
        }
    }
    function rebound() {
        snapAnimation.snapTo(0);
    }

    // expansion
    Component.onCompleted: internals.completed = true
    state: (internals.completed && styledItem.expansion.expanded) ? (listItemStyle.flickable ? "expandedWithFlickable" : "expandedNoFlickable") : ""
    states: [
        State {
            name: "expandedNoFlickable"
            PropertyChanges {
                target: styledItem
                height: styledItem.expansion.height
            }
        },
        State {
            name: "expandedWithFlickable"
            PropertyChanges {
                target: styledItem
                height: styledItem.expansion.height
            }
            PropertyChanges {
                target: listItemStyle.flickable
                // we do not need to restore the original values
                restoreEntryValues: false
                // and we should not get any binding updates even
                explicit: true
                contentY: {
                    var bottom = styledItem.y + styledItem.expansion.height - listItemStyle.flickable.contentY + listItemStyle.flickable.originY;
                    var dy = bottom - listItemStyle.flickable.height;
                    if (dy > 0) {
                        return listItemStyle.flickable.contentY + dy - listItemStyle.flickable.originY;
                    } else {
                        return listItemStyle.flickable.contentY;
                    }
                }
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "expandedWithFlickable"
            reversible: true
            enabled: listItemStyle.animatePanels
            ParallelAnimation {
                UbuntuNumberAnimation {
                    target: listItemStyle.flickable
                    property: "contentY"
                }
                UbuntuNumberAnimation {
                    target: styledItem
                    property: "height"
                }
            }
        },
        Transition {
            from: ""
            to: "expandedNoFlickable"
            reversible: true
            enabled: listItemStyle.animatePanels
            UbuntuNumberAnimation {
                target: styledItem
                property: "height"
            }
        }
    ]
}
