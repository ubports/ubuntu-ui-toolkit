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
    name: "PopupsComposerSheetAPI"

    function test_show() {
        composerSheet.show()
    }

    function test_hide() {
        composerSheet.hide()
    }

    function test_title() {
        compare(composerSheet.title,"","title is not set by default")

        var newTitle = "Hello World!"
        composerSheet.title = newTitle
        compare(composerSheet.title,newTitle,"set/get")
    }

    function test_contentsWidth() {
        compare(composerSheet.contentsWidth,units.gu(64),"contentsWidth is 64 grid units by default")
    }

    function test_contentsHeight() {
        compare(composerSheet.contentsHeight,units.gu(40),"contentsHeight is 40 grid units by default")
    }

    function test_modal() {
        compare(composerSheet.modal, true, "Sheets are modal by default");
    }

    ComposerSheet {
        id: composerSheet
    }
}
