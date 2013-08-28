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
     name: "UnityThemeIconProvider"

     Image {
         id: img
         source: "image://theme/call-start"
         asynchronous: false
     }

     Image {
         id: missingImg
         source: "image://theme/definitely-missing-icon"
         asynchronous: false
     }


     Image {
         id: fallbackImg
         source: "image://theme/definitely-missing-icon,call-start"
         asynchronous: false
     }

     function test_load_images() {

         verify(img.status === Image.Ready, "Failed to load image.");
         verify(img.height > 0, "Image height 0.");

         verify(missingImg.status === Image.Error, "Missing image does not report an error.");

         verify(fallbackImg.status == Image.Ready, "Fallback image failed to load.");
         verify(fallbackImg.height > 0, "Image height is 0.");
     }

}
