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

import QtQuick 2.2
import Ubuntu.Components 1.2

/*
  This component is the holder of the ListItem actions.
  */
Item {

    // styling properties
    /*
      Color of the background.
      */
    // FIXME: use Palette colors instead when available
    property color backgroundColor: (leading ? UbuntuColors.red : "white")

    /*
      Color used in coloring the icons.
      */
    // FIXME: use Palette colors instead when available
    property color foregroundColor: leading ? "white" : UbuntuColors.darkGrey

    /*
      Specifies the width of the component visualizing the action.
      */
    property real visualizedActionWidth: units.gu(2.5)

    // panel implementation
    id: panel
    width: Math.max(
               actionsRow.childrenRect.width,
               ListItemActions.visibleActions.length * MathUtils.clamp(visualizedActionWidth, height, actionsRow.maxItemWidth))

    // used for module/autopilot testing
    objectName: "ListItemPanel" + (leading ? "Leading" : "Trailing")

    /*
      Property holding the ListItem's contentItem instance
      */
    readonly property Item contentItem: parent ? parent.contentItem : null

    /*
      Specifies whether the panel is used to visualize leading or trailing actions.
      */
    readonly property bool leading: panel.ListItemActions.status == panel.ListItemActions.Leading

    anchors {
        left: contentItem ? (leading ? undefined : contentItem.right) : undefined
        right: contentItem ? (leading ? contentItem.left : undefined) : undefined
        top: contentItem ? contentItem.top : undefined
        bottom: contentItem ? contentItem.bottom : undefined
    }

    Rectangle {
        objectName: "panel_background"
        anchors {
            fill: parent
            // add 4 times the overshoot margins to cover the background when tugged
            leftMargin: (leading && panel.ListItemActions.listItem) ? -units.gu(4 * panel.ListItemActions.listItem.swipeOvershoot) : 0
            rightMargin: (!leading && panel.ListItemActions.listItem) ? -units.gu(4 * panel.ListItemActions.listItem.swipeOvershoot) : 0
        }
        color: panel.backgroundColor
    }

    // handle action triggering
    ListItemActions.onStatusChanged: {
        if (ListItemActions.status === ListItemActions.Disconnected && actionsRow.selectedAction) {
            actionsRow.selectedAction.trigger(actionsRow.listItemIndex >= 0 ? actionsRow.listItemIndex : null);
            actionsRow.selectedAction = null;
        }
    }

    // track drag direction, so we know in which direction we should snap
    property real prevX: 0.0
    property real snapChangerLimit: 0.0
    property real threshold: units.gu(1)
    property bool snapIn: false
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
    // default snapping!
    ListItemActions.onSwipingChanged: {
        if (ListItemActions.swiping) {
            // the dragging got started, set prevX
            prevX = panel.x;
            return;
        }
        if (!visible) {
            return;
        }
        // snap in if the offset is bigger than the overshoot and the direction of the drag is to reveal the panel
        var snapPos = (ListItemActions.offset > ListItemActions.listItem.swipeOvershoot && snapIn) ? panel.width : 0.0;
        ListItemActions.snapToPosition(snapPos);
    }

    Row {
        id: actionsRow
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: spacing
        }

        property real maxItemWidth: panel.parent ? (panel.parent.width / panel.ListItemActions.visibleActions.length) : 0

        property Action selectedAction
        property int listItemIndex

        Repeater {
            model: panel.ListItemActions.visibleActions
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
                    actionsRow.listItemIndex = panel.ListItemActions.listItemIndex;
                    panel.ListItemActions.snapToPosition(0.0);
                }

                Rectangle {
                    anchors.fill: parent
                    color: Theme.palette.selected.background
                    visible: pressed
                }

                Loader {
                    id: delegateLoader
                    height: parent.height
                    sourceComponent: panel.ListItemActions.container.delegate ? panel.ListItemActions.container.delegate : defaultDelegate
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
                width: panel.visualizedActionWidth
                height: width
                name: action.iconName
                color: panel.foregroundColor
                anchors.centerIn: parent
            }
        }
    }
}
