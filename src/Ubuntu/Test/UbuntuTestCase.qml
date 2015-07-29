/*
 * Copyright 2012, 2014 Canonical Ltd.
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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Components 1.2

/*!
    \qmltype UbuntuTestCase
    \inqmlmodule Ubuntu.Test 1.0
    \ingroup ubuntu-test
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

    /*!
      Find a non-visual child such as QtObject based on objectName.
      */
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
      Finds a visible child of an \a item having a given \a property with a given
      \a value.
      */
    function findChildWithProperty(item, property, value) {
        var childs = new Array(0);
        childs.push(item)
        while (childs.length > 0) {
            var child = childs[0];
            if (child.hasOwnProperty(property) && (child[property] === value)) {
                return child;
            }
            for (var i in childs[0].children) {
                childs.push(childs[0].children[i])
            }
            childs.splice(0, 1);
        }
        return null;
    }

    /*!
      Returns the center point of the \a item.
      */
    function centerOf(item) {
        if (!item) {
            return undefined;
        }

        var center = Qt.point(item.width / 2, item.height / 2);
        return center;
    }

    /*!
        Move Mouse from x,y to distance of dx, dy divided to steps with a stepdelay (ms).
    */
    function mouseMoveSlowly(item,x,y,dx,dy,steps,stepdelay) {
        mouseMove(item, x, y);
        var abs_dx = Math.abs(dx)
        var abs_dy = Math.abs(dy)
        var step_dx = dx / steps;
        var step_dy = dy /steps;

        var ix = 0;
        var iy = 0;

        for (var step=0; step < steps; step++) {
            if (Math.abs(ix) < abs_dx) {
                ix += step_dx;
            }
            if (Math.abs(iy) < abs_dy) {
                iy += step_dy;
            }
            mouseMove(item, x + ix, y + iy, stepdelay);
        }
    }

    /*!
      \qmlmethod UbuntuTestCase::flick(item, x, y, dx, dy, pressTimeout = -1, steps = -1, button = Qt.LeftButton, modifiers = Qt.NoModifiers, delay = -1)

      The function produces a flick event when executed on Flickables. When used
      on other components it provides the same functionality as \c mouseDrag()
      function. The optional \a pressTimeout parameter can be used to introduce
      a small delay between the mouse press and the first mouse move. Setting a
      negative or zero value will disable the timeout.

      The default flick velocity is built up using 5 move points. This can be altered
      by setting a positive value to \a steps parameter. The bigger the number the
      longer the flick will be. When a negative or zero value is given, the default
      of 5 move points will be used.

      \note The function can be used to select a text in a TextField or TextArea by
      specifying at least 400 millisecods to \a pressTimeout.
      */
    function flick(item, x, y, dx, dy, pressTimeout, steps, button, modifiers, delay) {
        if (item === undefined || item.x === undefined || item.y === undefined)
            return
        if (button === undefined)
            button = Qt.LeftButton
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (steps === undefined || steps <= 0)
            steps = 4;
        // make sure we have at least two move steps so the flick will be sensed
        steps += 1;
        if (delay === undefined)
            delay = -1;

        var ddx = dx / steps;
        var ddy = dy / steps;

        mousePress(item, x, y, button, modifiers, delay);
        if (pressTimeout !== undefined && pressTimeout > 0) {
            wait(pressTimeout);
        }
        // mouse moves are all processed immediately, without delay in between events
        mouseMoveSlowly(item, x, y, dx, dy, steps, delay);
        mouseRelease(item, x + dx, y + dy, button, modifiers);
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
      */
    function mouseLongPress(item, x, y, button, modifiers, delay) {
        mousePress(item, x, y, button, modifiers, delay);
        // the delay is taken from QQuickMouseArea, add few miliseconds to it to make sure we have
        // the long press triggered
        wait(900);
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

    /*!
        Convenience function to allow typing a full string instead of single characters
    */
    function typeString(string) {
        for (var i = 0; i < string.length; i++) {
            keyClick(string[i]);
        }
    }

    /*!
      Warning message formatter, uses file name, line and column numbers to build up the message.
      */
    function warningFormat(line, column, message) {
        return util.callerFile() + ":" + line + ":" + column + ": " + message;
    }

    /*!
        Wait for animations of the header and the style inside the header to finish.
        The MainView that has the header that may animate must be passed as an argument.
     */
    function waitForHeaderAnimation(mainView) {
        var header = findChild(mainView, "MainView_Header");
        verify(header !== null, "Could not find header.");
        var headerStyle = findChild(header, "PageHeadStyle");
        verify(headerStyle !== null, "Could not find header style.");

        // Wait for the header to start to move:
        wait(50);
        // Wait for animation of the style inside the header (when pushing/popping):
        tryCompareFunction(function(){ return headerStyle.animating }, false);
        // Wait for the header to finish showing/hiding:
        tryCompareFunction(function(){ return header.moving }, false);
    }
}
