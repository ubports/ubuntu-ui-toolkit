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
import Ubuntu.Components.Popups 1.0

TestCase {
    name: "PopupsDefaultSheetAPI"

    function test_show() {
        defaultSheet.show()
    }

    function test_hide() {
        defaultSheet.hide()
    }

    function test_title() {
        compare(defaultSheet.title,"","title is not set by default")

        var newTitle = "Hello World!"
        defaultSheet.title = newTitle
        compare(defaultSheet.title,newTitle,"set/get")
    }

    function test_contentsWidth() {
        compare(defaultSheet.contentsWidth,units.gu(64),"contentsWidth is 64 grid units by default")
    }

    function test_contentsHeight() {
        compare(defaultSheet.contentsHeight,units.gu(40),"contentsHeight is 40 grid units by default")
    }

    function test_doneButton() {
        compare(defaultSheet.doneButton,false,"doneButton is set to false by default")

        var newValue = true
        defaultSheet.doneButton = newValue
        compare(defaultSheet.doneButton,newValue,"set/get")
    }

    function test_modal() {
        compare(defaultSheet.modal, true, "Sheets are modal by default");
    }

    DefaultSheet {
        id: defaultSheet
    }
}
