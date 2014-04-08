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
	TestUtil {
		id:util
	}

	/*!
		Find a child from the item based on the objectName.
	*/
	function findChild(obj,objectName) {
		var childs = new Array(0);
		childs.push(obj)
		while (childs.length > 0) {
			if (childs[0].objectName == objectName) {
				return childs[0]
			}
			for (var i in childs[0].children) {
				childs.push(childs[0].children[i])
			}
			childs.splice(0, 1);
		}
		return null;
	}

	function findInvisibleChild(obj,objectName) {
		var childs = new Array(0);
		childs.push(obj)
		while (childs.length > 0) {
			if (childs[0].objectName == objectName) {
				return childs[0]
			}
			for (var i in childs[0].data) {
				childs.push(childs[0].data[i])
			}
			childs.splice(0, 1);
		}
		return null;
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

    /*!
      \qmlmethod UbuntuTestCase::flick(item, from, to, speed = -1, pressDelay = undefined)

      The function simulates a flick event over an \item. The flick is executed
      between \a from and \to points (built using Qt.point()) with a given \a speed
      (in msec) in between each intermediate point. A \a pressDelay will be executed
      between the mouse press event and the first mouse move if specified.

      \note The function can be used to select a text in a TextField or TextArea by
      specifying at least 400 millisecods to \a pressDelay.
      */
    function flick(item, from, to, speed, pressTimeout) {
        var pointCount = 5;
        if (from === undefined)
            qtest_fail("source point not defined", 2);
        if (to === undefined)
            qtest_fail("destination point not defined", 2);
        if (speed === undefined)
            speed = -1;
        else
            speed /= pointCount;

        var dx = to.x - from.x;
        var dy = to.y - from.y;

        mousePress(item, from.x, from.y);
        if (pressTimeout !== undefined) {
            wait(pressTimeout);
        }

        for (var i = 0; i < pointCount; i++) {
            mouseMove(item, from.x + (i + 1) * dx / pointCount, from.y + (i + 1) * dy / pointCount, speed);
        }
        mouseRelease(item, to.x, to.y);
        // empty event buffer
        wait(200);
    }

    /*!
        \qmlmethod UbuntuTestCase::mouseLongPress(item, x, y, button = Qt.LeftButton, modifiers = Qt.NoModifiers, delay = -1)

        Simulates a long press on a mouse \a button with an optional \a modifier
        on an \a item. The position is defined by \a x and \a y. If \a delay is
        specified, the test will wait the specified amount of milliseconds before
        the press.

        The position given by \a x and \a y is transformed from the co-ordinate
        system of \a item into window co-ordinates and then delivered.
        If \a item is obscured by another item, or a child of \a item occupies
        that position, then the event will be delivered to the other item instead.

        \sa mouseRelease(), mouseClick(), mouseDoubleClick(), mouseMove(), mouseDrag(), mouseWheel()
      */
    function mouseLongPress(item, x, y, button, modifiers, delay) {
        mousePress(item, x, y, button, modifiers, delay);
        // the delay is taken from QQuickMouseArea
        wait(800);
    }

    /*!
		Keeps executing a given parameter-less function until it returns the given
		expected result or the timemout is reached (in which case a test failure
		is generated)
	*/

	function tryCompareFunction(func, expectedResult, timeout) {
		var timeSpent = 0
		var success = false
		var actualResult
		if (timeout == undefined) {
                    timeout = 5000;
                }
		while (timeSpent < timeout && !success) {
			actualResult = func()
			success = qtest_compareInternal(actualResult, expectedResult)
			if (success === false) {
				wait(50)
				timeSpent += 50
			}
		}

		var act = qtest_results.stringify(actualResult)
		var exp = qtest_results.stringify(expectedResult)
		if (!qtest_results.compare(success,
				"function returned unexpected result",
				act, exp,
				util.callerFile(), util.callerLine())) {
			throw new Error("QtQuickTest::fail")
		}
	}
} 

