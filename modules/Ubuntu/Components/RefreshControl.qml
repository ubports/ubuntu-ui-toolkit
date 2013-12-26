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
*/

StyledItem {
    id: control

    property var target

    property string refreshMethod

    property string pullMessageString: i18n.tr("Pull and hold to refresh...")
    property string releaseMessageString: i18n.tr("Release to refresh...")

    function refreshBegins() {
//        pull.flickable.contentY = -units.gu(4);
    }
    function refreshEnds() {
//        pull.flickable.contentY = 0;
    }

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    height: units.gu(5)
    anchors {
        left: parent.left
        right: parent.right
    }
    y: __styleInstance ? __styleInstance.layoutY : 0

    // catch when to update
    Connections {
        target: control.parent
        onMovementEnded: {
            if (__styleInstance.puller) {
                // refresh target
                control.refreshBegins();
                control.target[control.refreshMethod]();
            }
            __styleInstance.stop();
        }
        onFlickEnded: print("flick ends")
    }
}
