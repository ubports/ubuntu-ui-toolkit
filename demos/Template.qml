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
import Ubuntu.Components 0.1

Rectangle {
    id: template

    width: 800
    height: 600

    property string title
    default property Item content

    onContentChanged: {
        content.parent = page
        content.anchors.top = header.bottom
        content.anchors.bottom = page.bottom
        content.anchors.left = page.left
        content.anchors.right = page.right
    }

    color: "#e6e6e6"

    Item {
        id: page

        parent: template
        anchors {
            fill: parent
            margins: 35
        }

        Binding {
            target: content.anchors
            property: "topMargin"
            value: page.anchors.margins
        }

        Item {
            id: header

            height: childrenRect.height
            anchors {
                left: parent.left
                right: parent.right
            }

            TextCustom {
                id: title
                ItemStyle.styleClass: "title"
                text: template.title
            }

            Rectangle {
                id: underline

                anchors {
                    top: title.bottom
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                }
                height: 1
                color: "#757373"
            }
        }
    }
}
