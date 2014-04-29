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

    function test_contentWidth() {
        expectFail("", "Content width is wrong due to rootItem not being initialized.");
        compare(dialog.contentWidth, units.gu(40), "Content width is 40 GU");
    }

    function test_edgeMargins() {
        compare(dialog.edgeMargins, units.gu(2), "Default edgeMargins are 2 GU");
    }

    function test_callerMargin() {
        compare(dialog.callerMargin, units.gu(1), "Default callerMargin is 1 GU");
    }

    function test_modal() {
        compare(dialog.modal, true, "Dialogs are modal by default");
    }

    Dialog {
        id: dialog
    }
}
