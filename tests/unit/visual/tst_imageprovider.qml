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
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

Item {
    width: units.gu(50)
    height: units.gu(100)

    Column {
        Image {
            id: test
            width: units.gu(20)
            height: units.gu(20)
            source: "/usr/share/icons/ubuntu-mobile/actions/scalable/add.svg"
            visible: source != ""
        }
        Image {
            id: bindingTest
            width: units.gu(20)
            height: units.gu(20)
            visible: source != ""
        }
    }

    UbuntuTestCase {
        name: "ImageExtension"
        when: windowShown

        SignalSpy {
            id: sourceChangeSpy
            signalName: "sourceChanged"
        }

        SignalSpy {
            id: visibleChangedSpy
            signalName: "visibleChanged"
        }

        function cleanup() {
            sourceChangeSpy.clear();
            visibleChangedSpy.clear();
        }

        // tests adjusted to reproduce bug #1401920
        function test_sourceChanged_bug1401920_data() {
            return [
                {tag: "Bitmap file", file: 'tst_icon-select.png'},
                {tag: "Bitmap file(18gu)", gu: 18, file: 'tst_icon-select.png'},
                {tag: "Bitmap with fragment", file: 'tst_icon-select.png#' + Date.now()},
                {tag: "Bitmap with fragment(18gu)", gu: 18, file: 'PageHeaderBaseDividerBottom@18.png#' + Date.now()},
                {tag: "Bitmap file with gu", file: 'tst_icon-select@18.png'},
                {tag: "Bitmap file with gu(18gu)", gu: 18, file: 'PageHeaderBaseDividerBottom@18.png'},
                {tag: "Bitmap with gu/ fragment", file: 'tst_icon-select@18.png#' + Date.now()},
                {tag: "Bitmap with gu/ fragment(18gu)", gu: 18, file: 'PageHeaderBaseDividerBottom@18.png#' + Date.now()},
                {tag: "Scalable file", file: 'battery-100-charging.svg'},
                {tag: "Scalable file(18gu)", gu: 18, file: 'shape.svg'},
                {tag: "Scalable with fragment", file: 'shape.svg#' + Date.now()},
                {tag: "Scalable with fragment(18gu)", gu: 18, file: 'battery-100-charging.svg#' + Date.now()},
            ];
        }

        function test_sourceChanged_bug1401920(data) {
            units.gridUnit = data.gu ? data.gu : 8;
            sourceChangeSpy.target = test;
            test.source = Qt.resolvedUrl(data.file);
            sourceChangeSpy.wait(400);
            compare(test.source, Qt.resolvedUrl(data.file));
        }

        function test_sourceNOTIFYable() {
            /* Test source through visible to cover NOTIFYable errors */
            visibleChangedSpy.target = bindingTest;
            bindingTest.source = Qt.resolvedUrl('battery-100-charging.svg');
            visibleChangedSpy.wait();
        }
    }
}
