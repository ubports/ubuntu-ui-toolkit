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
import Ubuntu.Components 1.2

MainView {
    width: units.gu(40)
    height: units.gu(60)
    Component.onCompleted: pageStack.push(page1)

    PageStack {
        id: pageStack
    }

    Page {
        id: page1
        title: "one"
        Button {
            anchors.centerIn: parent
            text: "next"
            onClicked: pageStack.push(page2)
        }
    }
    Page {
        id: page2
        title: "two"
        visible: false

        head.backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
    }
}
