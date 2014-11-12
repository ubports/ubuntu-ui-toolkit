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
      Color of the background.
      */
    // FIXME: use Palette colors instead when available
    property color backgroundColor: (leadingPanel ? UbuntuColors.red : "white")

    /*
      Color used in coloring the icons.
      */
    // FIXME: use Palette colors instead when available
    property color foregroundColor: leadingPanel ? "white" : UbuntuColors.darkGrey

    // panel implementation
    id: panel
    width: optionsRow.childrenRect.width

    // for testing
    objectName: "ListItemPanel" + (leadingPanel ? "Leading" : "Trailing")

    /*
      Property holding the ListItem's contentItem instance
      */
    readonly property Item contentItem: parent ? parent.contentItem : null
    /*
      Specifies whether the panel is used to visualize leading or trailing options.
      */
    readonly property bool leadingPanel: panel.ListItemActions.status == ListItemActions.Leading

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

    Row {
        id: optionsRow
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: spacing
        }

        property real maxItemWidth: panel.parent ? (panel.parent.width / panel.ListItemActions.visibleActions.length) : 0

        Repeater {
            model: panel.ListItemActions.visibleActions
            AbstractButton {
                action: modelData
                enabled: action.enabled
                opacity: action.enabled ? 1.0 : 0.5
                width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, optionsRow.maxItemWidth)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                Loader {
                    id: delegateLoader
                    height: parent.height
                    sourceComponent: panel.ListItemActions.delegate ? panel.ListItemActions.delegate : defaultDelegate
                    property Action action: modelData
                    onItemChanged: {
                        // this is needed only for testing purposes
                        if (item && item.objectName === "") {
                            item.objectName = "list_option_" + index
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
                width: units.gu(2.5)
                height: width
                name: action.iconName
                color: panel.foregroundColor
                anchors.centerIn: parent
            }
        }
    }
}
