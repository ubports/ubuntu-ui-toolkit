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

    default property alias content: column.children
    property alias spacing: column.spacing
    property alias scrollable: templateFlickable.interactive
    property list<Action> trailingActions
    flickable: templateFlickable

    header: PageHeader {
        title: template.title
        flickable: layout.columns === 1 ? templateFlickable : null
        onFlickableChanged: exposed = true;
        trailingActionBar.actions: trailingActions
    }

    ScrollView {
        //anchors.fill: parent
        //anchors.topMargin: template.header.flickable ? 0 : template.header.height
        objectName: "TemplateScrollView"
        anchors {
            fill: parent
            topMargin: template.header.flickable ? 0 : template.header.height
        }
        Flickable {
            id: templateFlickable
            objectName: "TemplateFlickable"
            anchors.fill: parent
            topMargin: units.gu(2)
            bottomMargin: units.gu(2)
            contentHeight: column.height
            interactive: contentHeight > height
            Column {
                id: column
                spacing: units.gu(6)
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: units.gu(2)
            }
        }
    }
}
