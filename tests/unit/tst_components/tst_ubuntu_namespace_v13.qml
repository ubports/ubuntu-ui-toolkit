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
import QtTest 1.0
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3

Item {
    width: 100
    height: 62

    TestCase {

        function test_API() {
            verify(Ubuntu.hasOwnProperty("toolkitVersion"), "toolkitVersion is part of 1.3 API!");
            verify(Ubuntu.hasOwnProperty("toolkitVersionMajor"), "toolkitVersionMajor is part of 1.3 API!");
            verify(Ubuntu.hasOwnProperty("toolkitVersionMinor"), "toolkitVersionMajor is part of 1.3 API!");
            verify(Ubuntu.hasOwnProperty("version"), "version() is part of 1.3 API!");
        }

        function test_toolkitversionMajor()
        {
            compare(Ubuntu.toolkitVersionMajor, 1, "Wrong major version!");
        }

        function test_toolkitversionMinor()
        {
            compare(Ubuntu.toolkitVersionMinor, 3, "Wrong minor version!");
        }

        function test_toolkitversion()
        {
            compare(Ubuntu.toolkitVersion, 1 * 256 + 3, "Wrong composed version!");
        }

        function test_version()
        {
            compare(Ubuntu.version(1, 0), 1 * 256 + 0, "Wrong composit version for 1.0!");
            compare(Ubuntu.version(1, 1), 1 * 256 + 1, "Wrong composit version for 1.1!");
            compare(Ubuntu.version(1, 2), 1 * 256 + 2, "Wrong composit version for 1.2!");
            compare(Ubuntu.version(1, 3), 1 * 256 + 3, "Wrong composit version for 1.3!");
            compare(Ubuntu.version(2, 0), 2 * 256 + 0, "Wrong composit version for 2.0!");
            compare(Ubuntu.version(2, 2), 2 * 256 + 2, "Wrong composit version for 2.2!");
        }
    }
}
