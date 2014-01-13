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
    readonly property alias inProgress: internals.refreshInProgress

    property string pullMessageString: i18n.tr("Pull and hold to refresh...")
    property string releaseMessageString: i18n.tr("Release to refresh...")

    signal refresh()

    function complete() {
        internals.refreshInProgress = false;
    }

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    height: units.gu(5)
    anchors {
        left: target.left
        right: target.right
    }
    y: __styleInstance ? __styleInstance.layoutY : 0

    QtObject {
        id: internals
        property bool refreshInProgress: false
    }

    // catch when to update
    Connections {
        target: control.target
        onDraggingChanged: {
            if (!control.parent.dragging && __styleInstance.puller) {
                if (control.waitForCompletion) {
                    internals.refreshInProgress = true;
                }
                control.refresh();
            }
        }
    }
}
