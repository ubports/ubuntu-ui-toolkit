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
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "GIconProvider"

     function test_load_image_data() {
         return [
                     {
                         tag: "local file with ok data",
                         icon:Qt.resolvedUrl("../../../demos/demo_image.jpg"),
                         status: Image.Ready
                     },
                     {
                         tag: "local file does not exist",
                         icon:Qt.resolvedUrl("../../../demos/NOT_EXISTING_demo_image.jpg"),
                         status: Image.Error
                     },
                     {
                         tag: "with some random data",
                         icon: "ftp://sdkfjldsfjfjfldsjfljlskjfl329isflkmjvcx",
                         status: Image.Error
                     },
                     {
                         tag: "gicon file with ok data",
                         icon: "preferences-desktop-display",
                         status: Image.Ready
                     },
                     {
                         tag: "gicon file with invalid filename",
                         icon: "preferences-desktop-display-NOT_EXISTING",
                         status: Image.Error
                     }
                     /*
                       //TODO: Add a valid theme icon
                     {
                         tag: "gicon file from theme",
                         icon: "application-exit",
                         status: Image.Ready
                     }*/
                 ]
     }

     function test_load_image(data) {
         // with valid value
         var newValue = "image://gicon/" + data.icon
         img.source = newValue;
         compare(img.status,data.status,"Image has been set");
     }

     Image {
         id: img
     }
}
