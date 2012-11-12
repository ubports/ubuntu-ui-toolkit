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
import "internalPopupUtils.js" as InternalPopupUtils

PopupBase {
    id: popover
    default property alias buttons: buttonColumn.data

    property alias title: headerText.text
    property alias text: questionText.text

    property Item caller

    // theme
    property real edgeMargins: units.gu(2)
    property real callerMargins: units.gu(2)

    // private
    function updatePosition() {
        var pos = new InternalPopupUtils.CallerPositioning(foreground, pointer, popover, caller, edgeMargins, callerMargins);
        pos.auto();
    }

    Background {
        dim: true
        ephemeral: false
    }

    Pointer {
        id: pointer
        color: "grey"
        opacity: 0.9
        longAxis: 2*callerMargins
        shortAxis: callerMargins
    }

    Foreground {
        id: foreground
        color: "grey"
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
            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
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
            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
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

        onWidthChanged: popover.updatePosition()
        onHeightChanged: popover.updatePosition()
    }

    onCallerChanged: updatePosition()
    onWidthChanged: popover.updatePosition()
    onHeightChanged: popover.updatePosition()
}
