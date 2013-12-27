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

/*!
    \qmltype RefreshControl
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Pull-to-refresh component for Flickables to reload a model upon pull.

    When used with Flickable, set parent to the flickable so the control does
    not land in the contentItem of Flickable.
    \qml
    ListModel {
        id: listModel
        function refresh() {
            // [...]
        }
    }
    Flickable {
        id: flicker
        width: units.gu(10)
        height: units.gu(10)

        RefreshControl {
            parent: flicker
            model: someModel
            refreshMethod: "refresh"
        }

        contentWidth: rect.width
        contentHeight: rect.height
        Rectangle {
            id: rect
            width: units.gu(20)
            height: units.gu(20)
            color: "red"
        }
    }
    \endqml
*/

StyledItem {
    id: control

    property Flickable target: parent
    property var model

    property string refreshMethod

    property string pullMessageString: i18n.tr("Pull and hold to refresh...")
    property string releaseMessageString: i18n.tr("Release to refresh...")

    function refreshBegins() {
        __styleInstance.start();
    }
    function refreshEnds() {
        __styleInstance.stop();
    }

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    height: units.gu(5)
    anchors {
        left: target.left
        right: target.right
    }
    y: __styleInstance ? __styleInstance.layoutY : 0

    // catch when to update
    Connections {
        target: control.target
//        onMovementEnded: {
//            if (__styleInstance.puller) {
//                // refresh target
//                control.refreshBegins();
//                control.target[control.refreshMethod]();
//            }
//            __styleInstance.stop();
//        }
        onDraggingChanged: {
            if (!control.parent.dragging && __styleInstance.puller) {
                control.refreshBegins();
                control.model[control.refreshMethod]();
            }
//            __styleInstance.stop();
        }
    }
}
