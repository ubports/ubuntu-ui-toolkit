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
import Ubuntu.Components 1.3

Page {
    id: template

    default property alias content: layout.children
    property alias spacing: layout.spacing

    Flickable {
        id: flickable
        objectName: "TemplateFlickable"
        anchors.fill: parent
        anchors.topMargin: units.gu(2)
        anchors.bottomMargin: units.gu(2)
        contentHeight: layout.height
        interactive: contentHeight > height

        Column {
            id: layout
            spacing: units.gu(6)
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: units.gu(2)
        }
    }

    Scrollbar {
        id: sb
        objectName: "TemplateScrollbar"
        flickableItem: flickable
        property alias interactive: sb.__interactive
    }
}
