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

import QtQuick 2.2
import Ubuntu.Components 1.2

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    Page {
        title: "UbuntuListView"

        UbuntuListView {
            anchors.fill: parent

            ListItemOptions {
                id: stockOptions
                Action {
                    iconName: "edit"
                }
                Action {
                    iconName: "share"
                }
                Action {
                    iconName: "attachment"
                }
            }

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 20; i++)
                        append({data: "List item mode #" + i})
                }
            }

            delegate: ListItem {
                trailingOptions: stockOptions

                Label {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: modelData + " @index " + index
                }
            }
        }
    }
}
