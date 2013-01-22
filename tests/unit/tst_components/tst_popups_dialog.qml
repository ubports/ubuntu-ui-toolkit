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
import Ubuntu.Components.Popups 0.1

TestCase {
    name: "PopupsDialogAPI"

    function test_show() {
        dialog.show()
    }

    function test_hide() {
        dialog.hide()
    }

    function test_caller() {
        compare(dialog.caller,null,"caller is not set by default")
    }

    function test_text() {
        compare(dialog.text,"","text is not set by default")

        var newText = "Hello World!"
        dialog.text = newText
        compare(dialog.text,newText,"set/get")
    }

    function test_title() {
        compare(dialog.title,"","title is not set by default")

        var newTitle = "Hello World!"
        dialog.title = newTitle
        compare(dialog.title,newTitle,"set/get")
    }

    Dialog {
        id: dialog
    }
}
