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
  This component is the holder of the ListItem options.
  */
Item {

    // styling properties

    /*
      Panel's background color
      */
    // FIXME: use Palette colors instead when available
    property color backgroundColor: (leadingPanel ? UbuntuColors.red : "white")

    /*
      Color used in coloring the icons
      */
    // FIXME: use Palette colors instead when available
    property color foregroundColor: panel.leadingPanel ? "white" : UbuntuColors.darkGrey

    // panel implementation
    id: panel
    // width must be calculated based on the amount of visible actions, as Row width will change gradually
    // based on the progress of Repeater, and snapping in tests may occur earlier than all the action
    // visualizations are constructed
    width: Math.max(panel.ListItemActions.visibleActions.length * MathUtils.clamp(actionButtonWidth, height, optionsRow.maxItemWidth),
                    optionsRow.childrenRect.width)

    // for testing
    objectName: "ListItemPanel" + (leadingPanel ? "Leading" : "Trailing")

    /*
      Property holding the ListItem's contentItem instance
      */
    readonly property Item contentItem: parent ? parent.contentItem : null

    /*
      Specifies whether the panel is used to visualize leading or trailing options.
      */
    readonly property bool leadingPanel: panel.ListItemActions.status == panel.ListItemActions.Leading

    /*
      Configures the width of the icon visualizing the action.
      */
    property real actionButtonWidth: units.gu(2.5)

    anchors {
        left: contentItem ? (leadingPanel ? undefined : contentItem.right) : undefined
        right: contentItem ? (leadingPanel ? contentItem.left : undefined) : undefined
        top: contentItem ? contentItem.top : undefined
        bottom: contentItem ? contentItem.bottom : undefined
    }

    Rectangle {
        objectName: "panel_background"
        anchors {
            fill: parent
            // add 4 times the overshoot margins to cover the background when tugged
            leftMargin: leadingPanel ? -units.gu(4 * panel.ListItemActions.overshoot) : 0
            rightMargin: leadingPanel ? 0 : -units.gu(4 * panel.ListItemActions.overshoot)
        }
        color: panel.backgroundColor
    }

    // handle action triggering
    ListItemActions.onStatusChanged: {
        if (ListItemActions.status === ListItemActions.Disconnected && optionsRow.selectedAction) {
            if (optionsRow.selectedAction.parameterType === Action.None) {
                optionsRow.selectedAction.parameterType = Action.Integer;
            }
            optionsRow.selectedAction.trigger(optionsRow.listItemIndex >= 0 ? optionsRow.listItemIndex : null);
            optionsRow.selectedAction = null;
        }
    }

    // track drag dirrection, so we know in which direction we should snap
    property real prevX: 0.0
    property bool leftToRight: false
    onXChanged: {
        leftToRight = prevX < x;
        prevX = x;
    }
    // default snapping!
    ListItemActions.onFlickingChanged: {
        if (ListItemActions.flicking) {
            // the dragging got started, set prevX
            prevX = panel.x;
            return;
        }
        if (!visible) {
            return;
        }
        // snap in if the offset is bigger than the overshoot and the direction of the drag is to reveal the panel
        var snapPos = (ListItemActions.offset > ListItemActions.overshoot &&
                       ((leftToRight && leadingPanel) || (!leftToRight && !leadingPanel))) ? panel.width : 0.0;
        ListItemActions.snapToPosition(snapPos);
    }

    Row {
        id: optionsRow
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
                action: modelData
                objectName: modelData.objectName
                enabled: modelData.enabled
                opacity: modelData.enabled ? 1.0 : 0.5
                width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, optionsRow.maxItemWidth)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                function trigger() {
                    optionsRow.selectedAction = modelData;
                    optionsRow.listItemIndex = panel.ListItemActions.listItemIndex;
                    panel.ListItemActions.snapToPosition(0.0);
                }

                Loader {
                    id: delegateLoader
                    height: parent.height
                    sourceComponent: panel.ListItemActions.container.delegate ? panel.ListItemActions.container.delegate : defaultDelegate
                    property Action action: modelData
                    property int index: index
                }
            }
        }
    }

    Component {
        id: defaultDelegate
        Item {
            width: height
            Icon {
                width: actionButtonWidth
                height: width
                name: action.iconName
                color: panel.foregroundColor
                anchors.centerIn: parent
            }
        }
    }
}
