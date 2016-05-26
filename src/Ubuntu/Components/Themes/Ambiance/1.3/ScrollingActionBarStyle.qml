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
    implicitHeight: units.gu(5)
//    implicitWidth: units.gu(36) // 9 * defaultDelegate.width

    overflowIconName: "contextual-menu"

    // Unused with the standard action icon buttons, but may be used with a custom delegate.
    overflowText: "More"

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    defaultDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_button"
        height: parent ? parent.height : undefined
        action: modelData
    }

    defaultNumberOfSlots: 3


    Rectangle {
        id: listViewContainer
        anchors {
            fill: parent
        }
        color: "pink"
        opacity: 0.5

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

        ListView {
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: Math.min(listViewContainer.width, contentWidth)

            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            delegate: styledItem.delegate
            model: listViewContainer.visibleActions

            Component.onCompleted: {
                print("ListView completed. count = "+count+", width = "+width+", contentWidth = "+contentWidth)
            }
            onWidthChanged: print("width = "+width+", contentWidth = "+contentWidth+", count = "+count)
        }

    }
}
