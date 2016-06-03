/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 1.2

UbuntuTestCase {
    name: "InputHandlerTest"

    TextField {
        id: tf
    }

    function test_internalFocus() {
        var handler = findChild(tf, "input_handler");
        tf.focus = false
        handler.input.focus = false
        compare(tf.focus, false, "Text field doesn't have focus");
        compare(handler.input.focus, false, "Input doesn't have focus");
        tf.focus = true
        compare(tf.focus, true, "Focus restored to text field");
        compare(handler.input.focus, true, "Focus automatically restored to input handler");
    }
}
