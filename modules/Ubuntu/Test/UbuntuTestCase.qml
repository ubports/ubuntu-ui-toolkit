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

/*!
    \qmlabstract UbuntuTestCase
    \inqmlmodule Ubuntu.Test 0.1
    \ingroup ubuntu
    \brief The UbuntuTestCase class expands the default TestCase class.

    \b{This component is under heavy development.}

    This class extends the default QML TestCase class which is available in QtTest 1.0.
*/
TestCase {

	/*!
		Find a child from the item based on the objectName.
	*/
    function findChild(obj,objectName) {
        var childrenStack = new Array(0);
        childrenStack.push(obj)
        while (childrenStack.length > 0) {
            if (childrenStack[0].objectName === objectName) {
                return childrenStack[0]
            }
            for (var i in childrenStack[0].children) {
                childrenStack.push(childrenStack[0].children[i])
            }
            childrenStack.splice(0, 1);
        }
        return undefined;
    }

	/*!
		Move Mouse from x,y to distance of dx, dy divided to steps with a stepdelay (ms).
	*/
	function mouseMoveSlowly(item,x,y,dx,dy,steps,stepdelay) {
		mouseMove(item,x,y);
		var step_dx = dx/steps;
		var step_dy = dy/steps;

		var ix = 0;
		var iy = 0;
		for (var step=0; step<steps; step++) {
			if (ix<dx) {
				ix += step_dx;
			}
			if (iy<dx) {
				iy += step_dy;
			}
			mouseMove(item,x + ix,y + iy,stepdelay);
		}
	}
} 

