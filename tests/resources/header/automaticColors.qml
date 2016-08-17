/*
 * Copyright (C) 2013-2015 Canonical Ltd.
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
import Ubuntu.Components 1.3

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(70)

    Page {
        title: "Colors"

        head.actions: [
            Action {
                iconName: "close"
                text: "one"
            },
            Action {
                iconName: "close"
                text: "two"
            },
            Action {
                iconName: "close"
                text: "three"
            },
            Action {
                iconName: "close"
                text: "four"
            }
        ]

        Column {
            height: childrenRect.height
            width: parent.width - units.gu(8)
            spacing: units.gu(1)
            anchors.centerIn: parent

            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Light"
                onTriggered: {
                    mainView.backgroundColor = "#eeeeee";
                    mainView.headerColor = mainView.backgroundColor;
                    mainView.footerColor = mainView.backgroundColor;
                }
            }
            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Dark"
                onTriggered: {
                    mainView.backgroundColor = "#333333";
                    mainView.headerColor = mainView.backgroundColor;
                    mainView.footerColor = mainView.backgroundColor;
                }
            }
            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Gradient"
                onTriggered: {
                    mainView.backgroundColor = "#6A69A2";
                    mainView.headerColor ="#343C60";
                    mainView.footerColor = "#8896D5";
                }
            }
        }
    }
}
