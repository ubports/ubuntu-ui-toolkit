/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.Styles 1.3 as Style

Style.ActionBarStyle {
    id: actionBarStyle
    implicitWidth: actionsContainer.implicitWidth
    implicitHeight: units.gu(5)

    overflowIconName: "contextual-menu"

    // Unused with the standard action icon buttons, but may be used with a custom delegate.
    overflowText: "More"

    backgroundColor: theme.palette.normal.background
    buttons {
        foregroundColor: theme.palette.normal.backgroundText
        pressedForegroundColor: buttons.foregroundColor
        disabledForegroundColor: theme.palette.disabled.backgroundText
        backgroundColor: "transparent" // action bar background is already colored
        pressedBackgroundColor: theme.palette.highlighted.background
        disabledBackgroundColor: buttons.backgroundColor
    }

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    defaultDelegate: Component {
        AbstractButton {
            id: button
            style: IconButtonStyle {
                foregroundColor: button.pressed ?
                                     actionBarStyle.buttons.pressedForegroundColor :
                                     button.enabled ?
                                         actionBarStyle.buttons.foregroundColor :
                                         actionBarStyle.buttons.disabledForegroundColor
                backgroundColor: button.pressed ?
                                     actionBarStyle.buttons.pressedBackgroundColor :
                                     button.enabled ?
                                         actionBarStyle.buttons.backgroundColor :
                                         actionBarStyle.buttons.disabledBackgroundColor
            }
            objectName: action.objectName + "_button"
            height: parent ? parent.height : undefined
            action: modelData
        }
    }

    defaultNumberOfSlots: 3

    Rectangle {
        id: background
        anchors.fill: parent
        color: actionBarStyle.backgroundColor
    }

    Component {
        id: fadeInComponent
        SequentialAnimation {
            id: fadeIn
            property alias target: opacityAnimation.target
            ScriptAction {
                script: fadeIn.target.opacity = 0.0;
            }
            UbuntuNumberAnimation {
                id: opacityAnimation
                from: 0.0
                to: 1.0
                property: "opacity"
                alwaysRunToEnd: true
                duration: UbuntuAnimation.BriskDuration
            }
            ScriptAction {
                script: fadeIn.destroy()
            }
        }
    }

    Row {
        id: actionsContainer

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
        function getReversedActions(actions) {
            var newlist = [];
            for (var i=actions.length-1; i >= 0; i--) {
                newlist.push(actions[i]);
            }
            return newlist;
        }

        property var directActions: getReversedActions(visibleActions.slice(0, numberOfSlots.used))
        property var barActions: overflowAction.visible
                                 ? directActions.concat(overflowAction)
                                 : directActions
        property var overflowActions: visibleActions.slice(numberOfSlots.used,
                                                           numberOfSlots.requested)

        QtObject {
            id: numberOfSlots
            property int requested: actionsContainer.visibleActions.length
            property int available: styledItem.numberOfSlots
            property int overflow: overflowAction.visible ? 1 : 0
            // when numberOfSlots < 1, show the overflow button, but set used
            // to 0 so that all actions will appear in the overflow panel.
            property int used: Math.min(Math.max(0, available - overflow), requested)
        }

        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        Repeater {
            id: actionsRepeater
            objectName: "actions_repeater"
            model: actionsContainer.barActions
            delegate: styledItem.delegate
            property int previousCount: count
            onCountChanged: {
                // after all itemAdded signals
                previousCount = count;
            }

            function fadeIn(item) {
                var fadeObject = fadeInComponent.createObject(actionBarStyle,
                                                              {"target": item});
                fadeObject.target = item;
                fadeObject.start();
            }
            onItemAdded: {
                if (count <= previousCount) return; // no items added
                if (index == 0) fadeIn(item);
            }
        }

        Action {
            id: overflowAction
            iconSource: actionBarStyle.overflowIconSource
            iconName: actionBarStyle.overflowIconName
            text: actionBarStyle.overflowText
            objectName: "overflow_action"
            visible: numberOfSlots.requested > numberOfSlots.available
            onTriggered: {
                var overflowButton = actionsRepeater.itemAt(actionsRepeater.count - 1);
                PopupUtils.open(actionsOverflowPopoverComponent, overflowButton);
            }
        }

        Component {
            id: actionsOverflowPopoverComponent
            OverflowPanel {
                id: actionsOverflowPopover
                objectName: "actions_overflow_panel"

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
                actions: actionsContainer.overflowActions
            }
        }
    }
}
