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
    name: "PopupsPopoverAPI"

    function test_show() {
        popOver.show()
    }

    function test_hide() {
        popOver.hide()
    }

    function test_caller() {
        compare(popOver.caller,null,"caller is not set by default")
    }

    //PopupBase properties
    function test_dismissArea() {
        compare(popOver.dismissArea, popOver, "Default sensing area is the popover");
    }

    function test_grabDismissAreaEvents() {
        compare(popOver.grabDismissAreaEvents, true, "Default grabs dismiss area events");
    }

    // Popover properties
    function test_contentWidth() {
        expectFail("", "Content width is wrong due to rootItem not being initialized.");
        compare(popOver.contentWidth, units.gu(40), "Content width is 40 GU");
    }

    function test_contentHeight() {
        expectFail("", "Content height is wrong due to rootItem not being initialized.");
        compare(popOver.contentHeight, units.gu(2), "Content height is 1.9 GUs with the default theme");
    }

    function test_pointerTarget() {
        compare(popOver.pointerTarget, popOver.caller, "pointerTarget is the same as caller");
    }

    function test_edgeMargins() {
        compare(popOver.edgeMargins, units.gu(2), "Default edgeMargins are 2 GU");
    }

    function test_callerMargin() {
        compare(popOver.callerMargin, 0, "Default callerMargin is 0");
    }

    function test_autoClose() {
        compare(popOver.autoClose, true, "autoClose is set by default");
    }

    // contentHeight testing is not possible as it follows clientRect height

    Popover {
        id: popOver
        Text {
            text: "Hello Popover!"
        }
    }
}
