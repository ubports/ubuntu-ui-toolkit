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

    property bool waitForCompletion: true
    readonly property alias refreshing: internals.refreshing

    property string pullText: i18n.tr("Pull to refresh...")
    property string releaseText: i18n.tr("Release to refresh...")

    signal refresh()

    function complete() {
        internals.refreshing = false;
    }

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    height: units.gu(5)
    anchors {
        left: target.left
        right: target.right
    }
    y: -(internals.contentY + __styleInstance.layoutHeight)

    Item {
        id: internals
        property bool refreshing: false
        property bool triggerRefresh: false
        property real contentY: target.contentY - target.originY
        property real threshold: control.__styleInstance.flipThreshold

        states: [
            State {
                name: "release-to-refresh"
                when: (internals.contentY < -internals.threshold) && !internals.refreshing
                PropertyChanges {
                    target: internals
                    triggerRefresh: true
                }
            },
            State {
                name: "refresh-in-progress"
                when: internals.refreshing && control.waitForCompletion
            }
        ]
        onStateChanged: control.__styleInstance.state = state
    }

    // catch when to update
    Connections {
        target: control.target
        onDraggingChanged: {
            if (!control.parent.dragging && internals.triggerRefresh) {
                if (control.waitForCompletion) {
                    internals.refreshing = true;
                }
                control.refresh();
            }
        }
    }
}
