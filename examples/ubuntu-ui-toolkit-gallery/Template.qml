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
    property alias scrollable: flickable.interactive

    header: PageHeader {
        title: template.title
        flickable: layout.columns === 1 ? flickable : null
        onFlickableChanged: exposed = true;
    }

    ScrollView {
        anchors.fill: parent
        anchors.topMargin: template.header.flickable ? 0 : template.header.height
        Flickable {
            id: flickable
            objectName: "TemplateFlickable"
            anchors.fill: parent
            contentHeight: column.height
            contentWidth: column.width
            interactive: contentHeight > height
            Column {
                id: column
                spacing: units.gu(6)
                width: flickable.width
                height: childrenRect.height
                //anchors.margins: units.gu(2)
            }
        }
    }

    //FIXME: gallery AP test expects a Scrollbar with TemplateScrollbar objectName, but we don't have that anymore!
    //    Scrollbar {
    //        id: sb
    //        objectName: "TemplateScrollbar"
    //        flickableItem: flickable
    //        property alias interactive: sb.__interactive
    //        __alwaysOnScrollbars: true
    //    }
}
