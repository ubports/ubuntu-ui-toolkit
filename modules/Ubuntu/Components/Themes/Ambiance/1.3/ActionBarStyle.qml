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
import Ubuntu.Components.Popups 1.0
//import Ubuntu.Components.Styles 1.2 as Style

Item {
    id: actionBarStyle
    //FIXME TIM: margins? no

    implicitWidth: actionsContainer.implicitWidth
    implicitHeight: units.gu(5) //actionsContainer.height

    property real buttonWidth: units.gu(4)

    Row {
        id: actionsContainer

//        height: parent.height

        //auto
//        implicitWidth: (numberOfSlots.used + numberOfSlots.overflow) * actionBarStyle.buttonWidth

        property var visibleActions: getVisibleActions(styledItem.actions)
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
            property int available: styledItem.numberOfSlots
            property int overflow: actionsOverflowButton.visible ? 1 : 0
            // when numberOfSlots < 1, show the overflow button, but set used
            // to 0 so that all actions will appear in the overflow panel.
            property int used: Math.min(Math.max(0, available - overflow), requested)
        }

        // FIXME TIM: anchors
        anchors {
            fill: parent
//            top: parent.top
//            bottom: parent.bottom
//            right: rightAnchor.left
//            right: parent.right
//            rightMargin: actionsContainer.width > 0 ? units.gu(1) : 0
        }
//        width: childrenRect.width
//        height: headerStyle.contentHeight
//        height: units.gu(5)

        Repeater {
            model: numberOfSlots.used
            PageHeadButton {
                id: actionButton
                // FIXME: replace _header by ActionBar objectName.
                objectName: action.objectName + "_header_button"
                action: actionsContainer.visibleActions[index]
//                color: headerStyle.buttonColor
//                color: "navy"

                // FIXME TIM: instead of using state, add delegate property later
//                state: styledItem.config.preset === "select" ?
//                           "IconAndLabel" : ""
            }
        }

        PageHeadButton {
            id: actionsOverflowButton
            // FIXME TIM: add ActionBar(Style) objectName here
            objectName: "actions_overflow_button"
            visible: numberOfSlots.requested > numberOfSlots.available
            // Ensure resetting of X when this button is not visible to avoid
            // miscalculation of actionsContainer.width. Fixes bug #1408481.
            onVisibleChanged: if (!visible) x = 0
            iconName: "contextual-menu"
            onTriggered: PopupUtils.open(actionsOverflowPopoverComponent, actionsOverflowButton)

            Component {
                id: actionsOverflowPopoverComponent

                OverflowPanel {
                    id: actionsOverflowPopover
                    // FIXME TIM: Rename
                    objectName: "actions_overflow_popover"

                    backgroundColor: "white"
//                    backgroundColor: headerStyle.panelBackgroundColor
//                    foregroundColor: headerStyle.panelForegroundColor
//                    highlightColor: headerStyle.panelHighlightColor

                    // TODO TIM: Verify that the bug doesn't come back

                    // Ensure the popover closes when actions change and
                    // the list item below may be destroyed before its
                    // onClicked is executed. See bug
                    // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1326963
                    Connections {
                        target: styledItem
                        onActionsChanged: {
                            actionsOverflowPopover.hide();
                        }
                    }
//                    Connections {
//                        target: styledItem
//                        onConfigChanged: {
//                            actionsOverflowPopover.hide();
//                        }
//                    }

                    actions: actionsContainer.visibleActions.slice(numberOfSlots.used,
                                                                 numberOfSlots.requested)
                }
            }
        }
    }

}
