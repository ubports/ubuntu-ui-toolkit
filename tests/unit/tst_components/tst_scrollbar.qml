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
import Ubuntu.Components 1.1

TestCase {
    name: "ScrollbarAPI"

    function initTestCase() {
    }

    function test_a()
    {
        compare(scrollbar.flickableItem, null, "Scrollbar does not have flickableItem set by default")
        var defaultAlign = Qt.AlignRight
        compare(scrollbar.align, defaultAlign, "Scrollbar is aligned to the right by default")
    }

    function test_flickableItem_Flickable()
    {
        scrollbar.flickableItem = flickable
        compare(scrollbar.flickableItem, flickable, "Cannot get/set flickableItem property")
    }

    function test_flickableItem_ListView()
    {
        scrollbar.flickableItem = listView
        compare(scrollbar.flickableItem, listView, "Cannot get/set flickableItem property")
    }

    function test_align_left()
    {
        scrollbar.align = Qt.AlignLeft
        compare(scrollbar.align, Qt.AlignLeft, "Cannot get/set align property")
    }

    function test_align_top()
    {
        scrollbar.align = Qt.AlignTop
        compare(scrollbar.align, Qt.AlignTop, "Cannot get/set align property")
    }

    function test_align_right()
    {
        scrollbar.align = Qt.AlignRight
        compare(scrollbar.align, Qt.AlignRight, "Cannot get/set align property")
    }

    function test_align_bottom()
    {
        scrollbar.align = Qt.AlignBottom
        compare(scrollbar.align, Qt.AlignBottom, "Cannot get/set align property")
    }

    function test_nonFlickable() {
        var failed = false
        try {
            scrollbar.flickableItem = nonFlickable
        } catch (e) {
            failed = true
        } finally {
            compare(failed,true,"can not set non-flickable item to flickableItem")
        }
    }

    Rectangle {
       id: nonFlickable
    }

    Flickable{
        id: flickable
    }

    ListView {
        id: listView
    }

    Scrollbar {
        id: scrollbar
        SignalSpy {
            id: signals
            signalName: "styleChanged"
        }
    }
}
