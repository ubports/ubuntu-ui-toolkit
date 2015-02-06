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

import QtQuick 2.3
import Ubuntu.Components.Styles 1.2 as Styles
import Ubuntu.Components 1.2

Styles.ListItemStyle {

    swipeOvershoot: units.gu(2)

    snapAnimation: UbuntuNumberAnimation {
        duration: UbuntuAnimation.SnapDuration
    }


    onParentChanged: print(parent, styledItem.divider.parent)
    // anchoring
    anchors {
        top: parent ? parent.top : undefined
        bottom: parent ? parent.bottom : undefined
        bottomMargin: styledItem.divider.visible ? styledItem.divider.height : 0
    }
    width: parent ? parent.width : 0

    // leading/trailing panels
    Component {
        id: panel
        ListItemPanel {}
    }

    Loader {
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.left
        }
        width: parent.width
        sourceComponent: styledItem.leadingActions && styledItem.leadingActions.actions.length > 0 ?
                             panel : null
        onItemChanged: {
            if (item) {
                item.actions = styledItem.leadingActions;
                item.leading = true;
            }
        }
    }
    Loader {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.right
        }
        width: parent.width
        sourceComponent: styledItem.trailingActions && styledItem.trailingActions.actions.length > 0 ?
                             panel : null
        onItemChanged: {
            if (item) {
                item.actions = styledItem.trailingActions;
                item.leading = false;
            }
        }
    }
}
