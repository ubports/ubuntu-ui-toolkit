/*
 * Copyright 2016 Canonical Ltd.
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
 *
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

UbuntuTestCase {
    when: windowShown

    //every test will assign the current view component (that is dynamically created) to this var
    //so that the cleanup() will destroy it (thus releasing mouse etc) even when a test fails midway
    property var currComponent: null

    //side == false -> set contentHeight
    //side == true ->  set contentWidth
    function setVeryLongContentItem(flickable, style, side) {
        if (!side) {
            flickable.contentHeight = flickable.height * 10 + 1
        } else {
            flickable.contentWidth = flickable.width * 10 + 1
        }

        //NOTE: THE TESTS ARE ASSUMING THAT WE DO THIS CHECK HERE
        //we have the check here because we want veryLongContentItem to be true
        //even, for instance, when we're checking a horizontal scrollbar and the very
        //long side is the height (so that when you have vert and hor scrollbars their
        //you can't have that one is in indicator style while the other is in thumb style)
        compare(style.veryLongContentItem, true, "Wrong veryLongContentItem value.")
    }

    function addContentMargins(flickable) {
        //note, these are content margins, not anchor margins
        flickable.topMargin = units.gu(2)
        flickable.bottomMargin = units.gu(3)
        flickable.leftMargin = units.gu(4)
        flickable.rightMargin = units.gu(5)

        compare(flickable.topMargin, units.gu(2), "Wrong topMargin")
        compare(flickable.bottomMargin, units.gu(3), "Wrong bottomMargin")
        compare(flickable.leftMargin, units.gu(4), "Wrong leftMargin")
        compare(flickable.rightMargin, units.gu(5), "Wrong rightMargin")
    }

    //this assumes we're not dealing with dynamic originX/Y
    //but it takes content margins into account
    function setContentPositionToTopLeft(flickable) {
        flickable.contentX = -flickable.leftMargin
        flickable.contentY = -flickable.topMargin
    }
    function setContentPositionToTopRight(flickable) {
        flickable.contentX = flickable.contentWidth + flickable.rightMargin - flickable.width
        flickable.contentY = -flickable.topMargin
    }
    function setContentPositionToBottomLeft(flickable) {
        flickable.contentX = -flickable.leftMargin
        flickable.contentY = flickable.contentHeight + flickable.bottomMargin - flickable.height
    }
    function setContentPositionToBottomRight(flickable) {
        flickable.contentX = flickable.contentWidth + flickable.rightMargin - flickable.width
        flickable.contentY = flickable.contentHeight + flickable.bottomMargin - flickable.height
    }

    function findInListProperty(listProperty, objectName) {
        for (var i = 0; i < listProperty.length; ++i) {
            if (listProperty[i].objectName === objectName)
                return listProperty[i]
        }
        return null
    }

    function setupSignalSpy(spy, target, signalName) {
        spy.clear()
        //reset signalName otherwise it will look for the old signalName in the new target
        spy.signalName = ""
        spy.target = target
        spy.signalName = signalName
    }

    function warningMsg(msg) {
        return testUtil.callerFile() + ": " + msg
    }

    //to promote code reuse
    function dragThumbForwardAndCheckPos_noRelease(scrollbar, flickable, firstStep, secondStep) {
        verify(scrollbar !== undefined, "Scrollbar reference must be valid.")
        verify(flickable !== undefined, "Flickable reference must be valid.")

        var thumb = getThumb(scrollbar)
        var style = scrollbar.__styleInstance
        var contentXBackup = flickable.contentX
        var contentYBackup = flickable.contentY

        compare(style.isScrollable, true, "Item is assumed to be scrollable.")

        setupSignalSpy(signalSpy, thumb, style.isVertical ? "onYChanged" : "onXChanged")
        mousePress(thumb, 0, 0)
        mouseMove(thumb,
                  (style.isVertical ? 0 : firstStep),
                  (style.isVertical ? firstStep : 0))
        mouseMove(thumb,
                  (style.isVertical ? 0 : secondStep),
                  (style.isVertical ? secondStep : 0))
        signalSpy.wait()

        if (style.isVertical) {
            console.log(flickable.contentY, contentYBackup)
            verify(flickable.contentY !== contentYBackup, "Check that mouse drag changed contentY." )
            compare(flickable.contentX, contentXBackup, "Vertical thumb drag must not change contentX")
        } else {
            verify(flickable.contentX !== contentXBackup, "Check that mouse drag changed contentX." )
            compare(flickable.contentY, contentYBackup, "Horizontal thumb drag must not change contentY")
        }

    }

    //check that the view moves by the amount defined in the styling
    //i.e. a small amount when clicking on steppers or arrow hw keys
    //and a bigger amount when clicking on the trough or PageUp/Down
    function checkScrolling(flickable, oldContentX, oldContentY, style,
                            longScrolling, direction, msgPrefix) {
        var verticalScrolling = style.isVertical
        var scrollingRatio = longScrolling ? style.longScrollingRatio : style.shortScrollingRatio
        var sizeProp = verticalScrolling ? flickable.height : flickable.width
        var directionFactor = direction < 0 ? -1 : 1
        var newContentProp = (verticalScrolling ? oldContentY : oldContentX)
                + directionFactor * sizeProp * scrollingRatio
        var contentPropName = verticalScrolling ? "contentY" : "contentX"
        var nonScrollingPropName = verticalScrolling ? "contentX" : "contentY"
        var errorMsg = msgPrefix + ": wrong " + contentPropName + "."

        tryCompare(flickable, contentPropName, newContentProp, 5000, errorMsg)
        wait(150)
        compare(flickable[contentPropName], newContentProp, errorMsg)

        //the other scrolling property should not be affected
        compare(flickable[nonScrollingPropName],
                (verticalScrolling ? oldContentX : oldContentY),
                msgPrefix + ": " + nonScrollingPropName + " should have not changed.")
    }

    function checkNoContentPositionChange(msgPrefix, flickable, expectedContentX, expectedContentY) {
        compare(flickable.contentX, expectedContentX, msgPrefix + " handling: unexpected contentX change.")
        compare(flickable.contentY, expectedContentY, msgPrefix + " handling: unexpected contentY change.")
    }

    function getScrollbarUtils(scrollbar) {
        var utils = findInvisibleChild(scrollbar.__styleInstance, "scrollbarUtils")
        verify(utils !== null, "Returning a valid reference to scrollbarUtils.")
        return utils
    }
    function findChildAndCheckValidInstance(object, childObjectName) {
        var obj = findChild(object, childObjectName)
        verify(obj !== null, "Returning a valid reference to the object with objectName " + childObjectName + ".")
        return obj
    }

    //move mouse over the scrollbar and wait for the transition to steppers style to complete
    function triggerSteppersMode(scrollbar) {
        var steppersTransition = findInListProperty(scrollbar.__styleInstance.transitions, "anythingToThumbSteppersTransition")
        verify(steppersTransition !== null, "Valid steppers transition instance found.")
        //trigger steppers style and wait for the transition to complete
        tryCompare(scrollbar.__styleInstance, "state", "hidden")
        mouseMove(scrollbar, 0, 0)
        tryCompare(steppersTransition, "running", true)
        tryCompare(scrollbar.__styleInstance, "state", "steppers")
        tryCompare(steppersTransition, "running", false)

        //This is hackish, but there doesn't seem to be a way around.
        //We have to wait for the Flow component to pick up the changes on steppers size
        //(which are now appearing due to the move to "steppers" state). A wait(0) will
        //force it to wait for the next event loop, where Flow will have already updated
        //the position of its children.
        wait(0)
    }

    //check that the scrollbar is using the hinting logic
    function checkHinting(scrollbar) {
        tryCompare(scrollbar.__styleInstance, "__hinting", true)
        tryCompare(scrollbar.__styleInstance, "state",
                   scrollbar.__styleInstance.veryLongContentItem ? "thumb" : "indicator")
        tryCompare(scrollbar.__styleInstance, "opacity", scrollbar.__styleInstance.overlayOpacityWhenShown)
    }

    //make sure the scrollbar is now scrollable
    function checkScrollableAndHidden(scrollbar) {
        tryCompare(scrollbar.__styleInstance, "isScrollable", true)
        tryCompare(scrollbar.__styleInstance, "state", "hidden")
        tryCompare(scrollbar.__styleInstance, "opacity", scrollbar.__styleInstance.overlayOpacityWhenHidden)
    }
    //make sure that the scrollbar is in the state it should be in
    //when the item is not scrollable
    //tryCompare is needed to avoid checking the properties before the related bindings are evaluated
    function checkNonScrollableState(scrollbar) {
        tryCompare(scrollbar.__styleInstance, "isScrollable", false)
        tryCompare(scrollbar.__styleInstance, "state", "")
        tryCompare(scrollbar.__styleInstance, "opacity", scrollbar.__styleInstance.overlayOpacityWhenHidden)
    }

    //defines how far the mouse has to be from the scrollbar before the drag is reset and temporarily disabled
    function dragResetThreshold(scrollbar) {
        return scrollbar.__styleInstance.troughThicknessSteppersStyle * 10
    }

    function clickInTheMiddle(item, button, modifiers, delay) {
        mouseClick(item, item.width/2, item.height/2, button, modifiers, delay)
    }

    //Centralize object lookups to avoid problems when changing object names
    function getFirstStepper(scrollbar) {
        return findChildAndCheckValidInstance(scrollbar.__styleInstance, "firstStepper")
    }
    function getSecondStepper(scrollbar) {
        return findChildAndCheckValidInstance(scrollbar.__styleInstance, "secondStepper")
    }
    function getTrough(scrollbar) {
        return findChildAndCheckValidInstance(scrollbar.__styleInstance, "trough")
    }
    function getThumb(scrollbar) {
        return findChildAndCheckValidInstance(scrollbar.__styleInstance, "interactiveScrollbarThumb")
    }
    function getThumbArea(scrollbar) {
        return findChildAndCheckValidInstance(scrollbar.__styleInstance, "thumbArea")
    }
    function getScrollAnimation(scrollbar) {
        var anim = findInvisibleChild(scrollbar.__styleInstance, "scrollAnimation")
        verify(anim !== null, "Returning a valid reference to scrollAnimation.")
        return anim
    }
}
