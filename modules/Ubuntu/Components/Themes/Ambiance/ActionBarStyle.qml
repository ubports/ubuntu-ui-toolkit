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
import Ubuntu.Components 1.3
//import Ubuntu.Components.Styles 1.2 as Style

Item {
    width: actionsContainer.width
    height: actionsContainer.height

    Row {
        id: actionsContainer

        property var visibleActions: getVisibleActions(styledItem.config.actions)
        function getVisibleActions(actions) {
            var visibleActionList = [];
            for (var i in actions) {
                var action = actions[i];
                if (action && action.hasOwnProperty("visible") && action.visible) {
                    visibleActionList.push(action);
                }
            }
            return visibleActionList;
        }

        QtObject {
            id: numberOfSlots
            property int requested: actionsContainer.visibleActions.length
            property int left: tabsButton.visible || backButton.visible ||
                               customBackButton.visible ? 1 : 0
            property int right: headerStyle.maximumNumberOfActions - left
            property int overflow: actionsOverflowButton.visible ? 1 : 0
            property int used: Math.min(right - overflow, requested)
        }

        anchors {
            top: parent.top
            right: rightAnchor.left
            rightMargin: actionsContainer.width > 0 ? units.gu(1) : 0
        }
        width: childrenRect.width
        height: headerStyle.contentHeight

        Repeater {
            model: numberOfSlots.used
            PageHeadButton {
                id: actionButton
                objectName: action.objectName + "_header_button"
                action: actionsContainer.visibleActions[index]
                color: headerStyle.buttonColor
                state: styledItem.config.preset === "select" ?
                           "IconAndLabel" : ""
            }
        }

        PageHeadButton {
            id: actionsOverflowButton
            objectName: "actions_overflow_button"
            visible: numberOfSlots.requested > numberOfSlots.right
            // Ensure resetting of X when this button is not visible to avoid
            // miscalculation of actionsContainer.width. Fixes bug #1408481.
            onVisibleChanged: if (!visible) x = 0
            iconName: "contextual-menu"
            color: headerStyle.buttonColor
            height: actionsContainer.height
            onTriggered: PopupUtils.open(actionsOverflowPopoverComponent, actionsOverflowButton)

            Component {
                id: actionsOverflowPopoverComponent

                OverflowPanel {
                    id: actionsOverflowPopover
                    objectName: "actions_overflow_popover"

                    backgroundColor: headerStyle.panelBackgroundColor
                    foregroundColor: headerStyle.panelForegroundColor
                    highlightColor: headerStyle.panelHighlightColor

                    // Ensure the popover closes when actions change and
                    // the list item below may be destroyed before its
                    // onClicked is executed. See bug
                    // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1326963
                    Connections {
                        target: styledItem.config
                        onActionsChanged: {
                            actionsOverflowPopover.hide();
                        }
                    }
                    Connections {
                        target: styledItem
                        onConfigChanged: {
                            actionsOverflowPopover.hide();
                        }
                    }

                    actions: actionsContainer.visibleActions.slice(numberOfSlots.used,
                                                                 numberOfSlots.requested)
                }
            }
        }
    }

}
