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

Template {
    title: "Popovers"

//    Column {
//        spacing: 30

//        TemplateRow {
//            title: "TODO"

//            Button {
//                text: "Pop!"
//            }
//        }
//    }

    Rectangle {
        color: "black"
        anchors.fill: parent
        id: canvas

        Component {
            id: button
            Button {
                text: "Pop!"
            }
        }

        Component.onCompleted: {
            var b = [];
            for (var i=0; i < 10; i++) b[i]
            var b1 = button.createObject(canvas);
            b1.anchors.top = canvas.top;
            b1.anchors.left = canvas.left;
        }
    }
}
