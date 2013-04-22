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

Item {
    width: 100
    height: 100

    property string path1
    property string path2

    Item {
        id: nonStyled
        Item {
            Label {
                id: testItem
                objectName: "testItem"
            }
        }
    }

    Item {
        ItemStyle.class: "parent"
        Item {
            id: nonStyledParent
        }
    }

    Item {
        ItemStyle.class: "other-parent"
        Item {
            id: nonStyledParent2
        }
    }

    Component.onCompleted: {
        nonStyled.parent = nonStyledParent
        path1 = testItem.ItemStyle.path
        nonStyled.parent = nonStyledParent2
        path2 = testItem.ItemStyle.path
    }
}
