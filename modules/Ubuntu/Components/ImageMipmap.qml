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

Image {
    id: image

    property url resource
    property real __scaleFactor: units.scaleFactor

    source: resource != "" ? __resolveResource(resource) : ""

    function __resolveResource(resourceUrl) {
        // FIXME: refine algorithm: do not try to load the resource @bucketName
        // if the file does not exist; instead load the highest possible resolution
        var resourceString = resourceUrl.toString()
        var baseName = resourceString.substring(0, resourceString.length - 4)
        var extension = resourceString.substring(resourceString.length - 4, resourceString.length)
        image.__scaleFactor = units.scaleFactor
        return baseName + "@" + units.bucketName + extension
    }

    smooth: true
    width: implicitWidth * __scaleFactor
    height: implicitHeight * __scaleFactor
}
