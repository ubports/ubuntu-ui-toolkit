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

import QtQuick 2.4
import Ubuntu.Components 1.3

Template {
    objectName: "crossFadeImagesTemplate"

    TemplateSection {
        id: crossFadeImageTemplateSection
        className: "CrossFadeImage"
        documentation: "qml-crossfadeimage.html"

        function next() {
            if (internal.index < internal.paths.length - 1)
                internal.index++;
            else
                internal.index = 0;
        }

        QtObject {
            id: internal
            property int index: 0
            property var paths: ["demo_image.jpg", "demo_image_2.png", "demo_image_3.png"]
            property url source: Qt.resolvedUrl(paths[index])
            property int fadeDuration: 1000
        }

        Timer {
            id: timer
            interval: 2000; running: true; repeat: true
            onTriggered: crossFadeImageTemplateSection.next()
        }

        Column {
            spacing: units.gu(2)
            anchors { left: parent.left; right: parent.right; }

            CrossFadeImage {
                width: parent.width
                height: units.gu(24)
                source: internal.source
                fadeStyle: "overlay"
                fadeDuration: internal.fadeDuration

                Label {
                    anchors.centerIn: parent
                    text: "fadeStyle: overlay"
                    color: "white"; style: Text.Raised; styleColor: "black"
                    z: 10
                }
            }
            CrossFadeImage {
                width: parent.width
                height: units.gu(24)
                source: internal.source
                fadeStyle: "cross"
                fadeDuration: internal.fadeDuration

                Label {
                    anchors.centerIn: parent
                    text: "fadeStyle: cross"
                    color: "white"; style: Text.Raised; styleColor: "black"
                    z: 10
                }
            }
        }
    }
}
