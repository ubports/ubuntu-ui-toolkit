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

import QtQuick 2.0
import Ubuntu.Components 1.2

MainView {
    width: units.gu(48)
    height: units.gu(60)

    // make sure we're not messed up by the deprecated toolbar
    useDeprecatedToolbar: false

    property string triggeredAction

    ListView {
        id: listView
        anchors.fill: parent
        model: 25
        delegate: ListItem {
            objectName: "listitem" + index
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "delete"
                        onTriggered: triggeredAction = iconName;
                    }
                ]
            }
            trailingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "search"
                        onTriggered: triggeredAction = iconName;
                    },
                    Action {
                        iconName: "edit"
                        onTriggered: triggeredAction = iconName;
                    },
                    Action {
                        iconName: "email"
                        onTriggered: triggeredAction = iconName;
                    }
                ]
            }
        }
    }

}
