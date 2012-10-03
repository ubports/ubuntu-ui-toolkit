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
import Ubuntu.Components 0.1

Row {
    id: templateRow

    property string title

    spacing: units.dp(10)
    height: units.dp(50)

    TextCustom {
        text: templateRow.title
        color: "#757373"
        width: units.dp(80)
        style: Text.Raised
        styleColor: "white"
    }

    // ensure that all the children are vertically centered
    onChildrenChanged: {
        for (var i=0; i<children.length; i++) {
            children[i].anchors.verticalCenter = templateRow.verticalCenter;
        }
    }
}
