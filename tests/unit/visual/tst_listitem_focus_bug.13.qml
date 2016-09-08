/*
 * Copyright 2016 Canonical Ltd.
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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import QtQuick.Window 2.2

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    UbuntuListView {
        id: testView
        anchors.fill: parent
        model: 50
        currentIndex: 10
        delegate: ListItem { width: testView.width; height: units.gu(7)
            objectName: "listItem" + index
            enabled: index > 20 && index < 25
            Label {
                anchors.centerIn: parent
                text: (parent.enabled ? "enabled" : "disabled") + index
            }
        }
    }

    UbuntuTestCase {
        when: windowShown
    }
}

