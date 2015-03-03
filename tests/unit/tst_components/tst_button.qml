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
    name: "ButtonAPI"

    function test_text() {
        compare(button.text,"","Button does not have text set by default")
        var newText = "Hello World!"
        button.text = newText
        compare(button.text,newText,"Can set/get text")
    }

    function test_action() {
        compare(actionButton.action, null, "Action is null by default")
        actionButton.action = action1
        compare(actionButton.action, action1, "action can be set")
        var newText = "Hello action!"
        action1.text = newText
        compare(actionButton.text, newText, "action can be used to define text")
        var newIcon = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
        action1.iconSource = newIcon
        compare(actionButton.iconSource, newIcon, "action can be used to define iconSource")
    }

    function test_iconPosition() {
        compare(button.iconPosition,"left","The default value for iconPosition is 'left'")
        var newIconPosition = "right"
        button.iconPosition = newIconPosition
        compare(button.iconPosition,newIconPosition)
    }

    function test_color() {
        var newColor = "#f00baa"
        button.color = newColor
        compare(button.color,newColor,"Can set/get color")
    }

    function test_font() {
        verify(button.font === Qt.font({family: "Ubuntu", pixelSize: FontUtils.sizeToPixels("medium")}), "Default font differs");
    }

    function test_hovered() {
        compare(button.hovered,false,"Hovered is boolean and false by default")
    }

    function test_iconSource() {
        compare(button.iconSource,"","iconSource is not set by default")
        var newIcon = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
        button.iconSource = newIcon
        compare(button.iconSource,newIcon,"Can set/get iconSource")
    }

    function test_pressed() {
        compare(button.pressed,false,"Pressed is boolean and false by default")
    }

    function test_signal_clicked() {
        signalSpy.signalName = "clicked";
        compare(signalSpy.valid,true,"clicked signal exists")
    }

    function test_signal_pressAndHold() {
        signalSpy.signalName = "pressAndHold";
        compare(signalSpy.valid,true,"pressAndHold signal exists")
    }

    Button {
        id: button
        SignalSpy {
            id: signalSpy
            target: parent
        }
    }

    // Use a new button for action tests, because other tests override
    //  the button's default text and iconSource so they are no longer
    //  automatically taken from the action.
    Button {
        id: actionButton
    }
    Action {
        id: action1
    }
}
