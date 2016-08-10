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

import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"
    useDeprecatedToolbar: true

    PageStack {
        id: pageStack
        Component.onCompleted: push(page0)

        Page {
            id: page0
            title: "Page 0"
            visible: false

            Button {
                objectName: "go_to_page1"
                text: "Go to page 1"
                onClicked: pageStack.push(page1)
            }
        }

        Page {
            id: page1
            title: "Page 1"
            visible: false
        }
    }
}
