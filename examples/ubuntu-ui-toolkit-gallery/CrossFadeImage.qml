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

import QtQuick 2.0
import Ubuntu.Components 1.1

Template {
    objectName: "crossFadeImagesTemplate"

    TemplateSection {
        id: crossFadeImageTemplateSection
        className: "CrossFadeImage"

        Component.onCompleted: timer.start()

        function next() {
            if (internal.index < internal.paths.length - 1)
                internal.index++;
            else
                internal.index = 0;
        }

        QtObject {
            id: internal
            property int index: 0
            property var paths: ["demo_image.jpg", "demo_image_2.jpg", "demo_image_3.jpg"]
        }

        Timer {
            id: timer
            interval: 500; running: true; repeat: true
            onTriggered: crossFadeImageTemplateSection.next()
        }

        Column {
            spacing: units.gu(2)

            CrossFadeImage {
                source: internal.paths[internal.index]
                fadeStyle: "overlay"
            }
            CrossFadeImage {
                source: internal.paths[internal.index]
                fadeStyle: "cross"
            }
        }
    }
}
