/*
 * Copyright 2016 Canonical Ltd.
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

AdaptivePageLayout {
    id: apl
    width: units.gu(80)
    height: units.gu(71)
    asynchronous: false

    primaryPage: Page {
        header: PageHeader {
            title: "Test"
        }
    }

    Component {
        id: testPage
        Page {
            objectName: "test_page"
            header: PageHeader {
                title: "NextPage"
            }
            property QtObject model : QtObject { objectName: "test" }
        }
    }

    UbuntuTestCase {
        when: windowShown

        function findPageFromLayout(apl, objectName) {
            var body = findChild(apl, "body");
            verify(body);
            return findChild(body, objectName);
        }

        function test_parameters_bug1604780() {
            apl.addPageToCurrentColumn(apl.primaryPage, testPage, {model: null});
            var page = findPageFromLayout(apl, "test_page");
            verify(page);
            compare(page.model, null);
        }
    }
}

