/*
 * Copyright 2014-2016 Canonical Ltd.
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
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Window {
    objectName: 'mainView'
    width: units.gu(48)
    height: units.gu(60)

    Page {
        anchors.fill: parent

        Button {
            objectName: "open_dialog"
            text: "Open Dialog"
            onClicked: PopupUtils.open(Qt.resolvedUrl("MyDialog.qml"), null, { "text": "The quick brown fox jumps over the lazy dog" })
        }
    }
}
