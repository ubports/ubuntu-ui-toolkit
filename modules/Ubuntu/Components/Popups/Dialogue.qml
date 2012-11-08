/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1
//import "../mathUtils.js" as MathUtils

PopupBase {
    id: popover
    default property alias buttons: buttonColumn.data

    property alias title: headerText.text
    property alias text: questionText.text

    // theme
    property real edgeMargins: units.gu(2)
    property real callerMargins: units.gu(0.5)

    // private
    function updatePosition(item) {
        // TODO: different behavior when the caller is a popover or sheet. Then fill the caller.
        var pos = new PopupUtils.Positioning(item, popover, caller, edgeMargins, callerMargins);

        var minWidth = item.width + 2*edgeMargins;
        var minHeight = item.height + 2*edgeMargins;
        // TODO: do specialized positioning on small screens.

        var coords;
        if (!popover.caller) {
            // TODO: ERROR
            coords = pos.center();
        } else {
            coords = pos.auto();
        }

        item.x = coords.x;
        item.y = coords.y;
    }

    Background {
        dim: true
        ephemeral: false
    }

    Foreground {
        id: foreground
        width: Math.min(units.gu(40), popover.width)
        height: MathUtils.clamp(childrenRect.height, units.gu(32), 3*popover.height/4)

        TextCustom {
            id: headerText
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            fontSize: "large"
            horizontalAlignment: Text.AlignHCenter
        }

        TextCustom {
            id: questionText
            anchors {
                top: headerText.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            width: parent.width - 2*anchors.margins
            fontSize: "medium"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }

        Column {
            id: buttonColumn
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            spacing: units.gu(1)

            height: childrenRect.height + anchors.bottomMargin

            onChildrenChanged: {
                for (var i = 0; i < children.length; i++) {
                    children[i].anchors.left = buttonColumn.left;
                    children[i].anchors.right = buttonColumn.right;
                }
            }
        }

        onWidthChanged: popover.updatePosition(foreground)
        onHeightChanged: popover.updatePosition(foreground)
    }

    onCallerChanged: updatePosition(foreground)
}
