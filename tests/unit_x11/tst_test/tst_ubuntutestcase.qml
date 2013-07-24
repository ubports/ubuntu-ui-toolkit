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
import Ubuntu.Test 0.1

Rectangle {
 id: root
 width: 800
 height: 600

 MouseArea {
    id: mouseArea
    objectName: "myMouseArea"
 	anchors.fill: parent
 	hoverEnabled: true
	property int testX : 0
	property int testY : 0
	property int steps : 0

 	onPositionChanged: {
 	   testX = mouseX;
	   testY = mouseY;
	   steps++;
 	}
 }
 
 UbuntuTestCase {
    name: "TestTheUbuntuTestCase"
    when: windowShown

    function test_mouseMoveSlowly() {
       mouseMoveSlowly(root,0,0,800,600,10,100);
       compare(mouseArea.testX,800);
       compare(mouseArea.testY,600);
       compare(mouseArea.steps,10);
    }

    function test_findChild() {
        var child = findChild(root,"myMouseArea");
        compare(child!==undefined,true, "When a child is found, it is returned");
        compare(child.objectName,"myMouseArea");

        child = findChild(root,"NoSuchChildHere");
        compare(child===undefined,true,"When there is no child, function should return undefined");
    }
 }
}
