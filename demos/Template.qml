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

import QtQuick 1.1
import "../Components"

Rectangle {
    id: template

    width: 800
    height: 600

    property string title
    default property Item content

    onContentChanged: {
        content.parent = page
        content.anchors.top = header.bottom
        content.anchors.topMargin = page.anchors.margins
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

        Item {
            id: header

            height: childrenRect.height
            anchors {
                left: parent.left
                right: parent.right
            }

            TextCustom {
                id: title

                fontSize: "x-large"
                text: template.title
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
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
