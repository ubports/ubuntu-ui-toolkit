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
import Ubuntu.Components 1.1

/*
  This component is the holder of the ListItem options.
  */
Item {
    id: panel
    width: optionsRow.childrenRect.width

    /*
      Property holding the ListItem's contentItem instance
      */
    readonly property Item contentItem: parent ? parent.contentItem : null

    /*
      Index of the ListItem, if the ListItem is inside a ListView or has been
      created using a Repeater.
      */
    property int listItemIndex: -1

    /*
      Specifies whether the panel is used to visualize leading or trailing options.
      */
    property bool leadingPanel: false
    /*
      The delegate to be used to visualize the options
      */
    property Component delegate

    /*
      Actions
      */
    property var actionList

    /*
      Emitted when action is triggered
      */
    signal selected()

    // fire selected action when parent is removed
    onParentChanged: {
        if (!parent && selectedAction) {
            selectedAction.triggered(listItemIndex >= 0 ? listItemIndex : null);
            selectedAction = null;
        }
    }
    property Action selectedAction: null

    anchors {
        left: contentItem ? (leadingPanel ? undefined : contentItem.right) : undefined
        right: contentItem ? (leadingPanel ? contentItem.left : undefined) : undefined
        top: contentItem ? contentItem.top : undefined
        bottom: contentItem ? contentItem.bottom : undefined
    }

    Rectangle {
        anchors.fill: parent
        // FIXME: use Palette colors instead when available
        color: leadingPanel ? UbuntuColors.red : "white"
    }

    Row {
        id: optionsRow
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: spacing
        }

        property real maxItemWidth: panel.parent ? (panel.parent.width / panel.actionList.actions.length) : 0

        Repeater {
            model: panel.actionList.actions
            AbstractButton {
                action: modelData
                visible: action.visible && action.enabled
                width: (!visible || !enabled) ?
                           0 : MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, optionsRow.maxItemWidth)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                function trigger() {
                    // save the action as we trigger when the rebound animation is over
                    // to make sure we properly clean up the blockade of teh Flickables
                    panel.selectedAction = action;
                    panel.selected();
                }

                Loader {
                    id: delegateLoader
                    height: parent.height
                    sourceComponent: panel.delegate ? panel.delegate : defaultDelegate
                    property Action action: modelData
                    property int index: index
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
                // FIXME: use Palette colors instead when available
                color: panel.leadingPanel ? "white" : UbuntuColors.darkGrey
                anchors.centerIn: parent
            }
        }
    }
}
