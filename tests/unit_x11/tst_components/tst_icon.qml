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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

Item {
    width: units.gu(40)
    height: units.gu(60)

    Icon {
        id: icon
        width: units.gu(2)
        height: units.gu(2)
    }

    UbuntuTestCase {
        name: "Icon"
        when: windowShown

        function cleanup() {
            icon.name = "";
        }

        function test_name() {
            icon.name = "foo";

            var image = findChild(icon, "image");
            compare(image.source, "image://theme/foo", "Source of the image should be image://theme/{name}.");
        }

        function test_source() {
            icon.name = "foo";
            icon.source = "file://foo/bar";

            var image = findChild(icon, "image");
            compare(image.source, "file://foo/bar", "Source of the image should equal icon.source.");
        }
    }
}
