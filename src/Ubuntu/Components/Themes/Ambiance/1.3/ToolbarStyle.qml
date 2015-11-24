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
import Ubuntu.Components.Styles 1.3 as Style

Style.ToolbarStyle {
    id: toolbarStyle
    implicitWidth: parent ? parent.width : 0
    implicitHeight: units.gu(4)

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    defaultDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_button"
        height: parent ? parent.height : undefined
        width: units.gu(4)
        action: modelData
    }

    Item {
        anchors {
            left: parent.left
            leftMargin: units.gu(1)
            top: parent.top
            bottom: parent.bottom
        }
        Repeater {
            // FIXME: A Loader may be enough here, but then we
            //  have no way to set the model (action) of the ActionButton.
            model: styledItem.fixedAction
            delegate: styledItem.delegate
        }
    }

    Row {
        id: actionsContainer
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            rightMargin: units.gu(1)
        }

        property var visibleActions: getVisibleActions(styledItem.actions)
        function getVisibleActions(actions) {
            var visibleActionList = [];
            for (var i in actions) {
                var action = actions[i];
                if (action && action.hasOwnProperty("visible") && action.visible) {
                    if (visibleActionList.length > 8) {
                        print("Toolbar currently only supports up to 9 actions.");
                        break;
                    }
                    visibleActionList.push(action);
                }
            }
            return visibleActionList;
        }

        Repeater {
            id: actionsRepeater
            objectName: "actions_repeater"
            model: actionsContainer.visibleActions
            delegate: styledItem.delegate
        }
    }
}
