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
import Ubuntu.Components.Popups 1.3

ActionSelectionPopover {
    objectName: 'listItemContextMenu'

    function mergeActions(to, from) {
        if (from == null)
            return;

        var actions = from.actions;
        for (var i in actions) {
            var action = actions[i];
            if (!action.text)
                action.text = action.iconName;
            to.push(actions[i]);
        }
    }

    actions: {
        var all = [];
        mergeActions(all, caller.leadingActions);
        mergeActions(all, caller.trailingActions);
        return all;
    }

    // Hide the arrow
    Binding {
        target: __foreground
        property: "direction"
        value: "none"
    }
}
