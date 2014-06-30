/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    useDeprecatedToolbar: false

    PageStack {
        id: stack
        Component.onCompleted: stack.push(page)

        Page {
            title: "Test title"
//            head.contents: Rectangle {
//                color: "red"
//                width: units.gu(10)
//                height: units.gu(5)
//            }

            id: page

            Button {
                anchors.centerIn: parent
                text: "push"
                onClicked: stack.push(pushMe)
            }
        }

        Page {
            title: "Invisible title"
            id: pushMe

            head {
//                actions: Action {
//                    iconName: "search"
//                }

                contents: Rectangle {
                    color: UbuntuColors.orange
                    height: units.gu(5)
//                    width: units.gu(9)
                    width: parent ? parent.width - units.gu(2) : undefined
                }
            }
        }
    }
}
