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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)


    Component {
        id: scrollViewComp
        Item {
            id: root
            //make sure that the item is smaller than the window otherwise we won't be able
            //to position the mouse on the scrollbar
            width: units.gu(40)
            height: units.gu(30)
            property alias scrollview: freshScrollView
            property alias content: content

            //tests will manually enable this property
            property bool innerItemConsumesKeyEvents: false

            Keys.enabled: true
            //Don't change this to a ListView, this usecase has to be simple,
            //we don't want the difficulties added by ListView, i.e. delegates
            //size estimation, dynamic contentWidth/Height, etc
            ScrollView {
                id: freshScrollView
                anchors.fill: parent
                clip: true
                StyledItem {
                    id: content
                    width: units.gu(100)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    activeFocusOnPress: true

                    Keys.enabled: true
                    Keys.onPressed: {
                        if (event.key === Qt.Key_PageDown && root.innerItemConsumesKeyEvents) {
                            event.accepted = true
                        }
                    }
                }
            }
        }
    }


    Component {
        id: nestedScrollViewComp
        Item {
            id: root
            width: main.width - units.gu(5)
            height: units.gu(50)
            property alias innerScrollView: innerScrollView
            property alias scrollview: freshScrollView
            property alias content: content

            Keys.enabled: true
            //Don't change this to a ListView, this usecase has to be simple,
            //we don't want the difficulties added by ListView, i.e. delegates
            //size estimation, dynamic contentWidth/Height, etc
            ScrollView {
                id: freshScrollView
                anchors.fill: parent
                clip: true
                Rectangle {
                    id: content
                    width: units.gu(50)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    color: "blue"

                    ScrollView {
                        id: innerScrollView

                        //make sure that the inner view is completely visible in the root item
                        width: root.width / 2
                        height: root.height / 2

                        Rectangle {
                            width: innerScrollView.width + units.gu(2)
                            height: innerScrollView.height + units.gu(2)
                            color: "yellow"
                        }
                    }
                }
            }
        }
    }

    Component {
        id: nonScrollableTestViewComp
        Item {
            id: root
            width: main.width - units.gu(5)
            height: units.gu(50)
            property alias scrollview: freshScrollView
            property alias content: content

            ScrollView {
                id: freshScrollView
                anchors.fill: parent
                clip: true
                Rectangle {
                    id: content

                    //make sure that it's not scrollable
                    width: freshScrollView.width / 2
                    height: freshScrollView.height / 2
                    color: "blue"
                }
            }
        }
    }

    SignalSpy {
        id: signalSpy
    }
    SignalSpy {
        id: anotherSignalSpy
    }
    SignalSpy {
        id: ultimateSignalSpy
    }

    Flickable {
        anchors.fill: parent
        Column {
            id: column
        }
    }

    ScrollbarTestCase {
        name: "ScrollView"

        //another ref to an object we want to destroy in cleanup()
        property var anotherDynamicComp: null

        function getFreshScrollView() {
            var wrapper = scrollViewComp.createObject(column)
            verify(wrapper !== null, "Error: dynamic item creation failed.")
            currComponent = wrapper
            return wrapper
        }
        function getHorizontalScrollbar(scrollView) {
            var scrollbar = findChild(scrollView, "horizontalScrollbar")
            verify(scrollbar !== null, "Could not find horizontal scrollbar.")
            return scrollbar
        }
        function getVerticalScrollbar(scrollView) {
            var scrollbar = findChild(scrollView, "verticalScrollbar")
            verify(scrollbar !== null, "Could not find vertical scrollbar.")
            return scrollbar
        }
        function focusScrollView(scrollview) {
            mouseClick(scrollview, 0, 0)
            tryCompare(scrollview, "focus", true, 5000, "Key events propagation: scrollview not focused.")
            tryCompare(scrollview, "activeFocus", true, 5000, "Key events propagation: scrollview not focused.")
        }
        function sendKeyAndCheckItHasNoEffect(keyName, keyCode, flickable, expectedContentX, expectedContentY) {
            keyClick(keyCode)
            wait(150)
            checkNoContentPositionChange(keyName + " key", flickable, expectedContentX, expectedContentY)
        }
        function cleanup() {
            if (currComponent) {
                currComponent.destroy()
                currComponent = null
            }
            if (anotherDynamicComp) {
                anotherDynamicComp.destroy()
                anotherDynamicComp = null
            }
            gc()
        }

        function test_veryLongContentItemIsTrueEvenWhenOnlyOneSideIsVeryLong_data() {
            return [
                        { tag: "OnlyHeight", setHeight: true, setWidth: false },
                        { tag: "OnlyWidth", setHeight: false, setWidth: true },
                        { tag: "Both", setHeight: true, setWidth: true }
                    ]
        }

        //this is to ensure that we have a consistent style for the scrollbars.
        //Even if only one side of the item is very long, we want both hor and vert scrollbars
        //to use thumb style, for instance.
        //The test that veryLongContentItem will trigger thumb style is inside tst_scrollbars.qml
        function test_veryLongContentItemIsTrueEvenWhenOnlyOneSideIsVeryLong(data) {
            var freshTestItem = getFreshScrollView()
            var scrollview = freshTestItem.scrollview
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var verticalScrollbar = getVerticalScrollbar(scrollview)

            scrollview.flickableItem.contentWidth = scrollview.flickableItem.width + 1
            scrollview.flickableItem.contentHeight = scrollview.flickableItem.height + 1
            compare(horizontalScrollbar.__styleInstance.veryLongContentItem, false, "Horizontal scrollbar: veryLongContentItem should be false.")
            compare(verticalScrollbar.__styleInstance.veryLongContentItem, false, "Vertical scrollbar: veryLongContentItem should be false.")

            //we call it twice just because we centralized the check on veryLongContentItem inside that method
            if (data.setHeight) {
                setVeryLongContentItem(scrollview.flickableItem, horizontalScrollbar.__styleInstance, false)
                setVeryLongContentItem(scrollview.flickableItem, verticalScrollbar.__styleInstance, false)
                //make it long enough to avoid timing issues with the flickable stopping before we manage
                //to check the variables
                if (!data.setWidth)
                    scrollview.flickableItem.contentWidth = scrollview.flickableItem.width * 4
            }

            if (data.setWidth) {
                setVeryLongContentItem(scrollview.flickableItem, horizontalScrollbar.__styleInstance, true)
                setVeryLongContentItem(scrollview.flickableItem, verticalScrollbar.__styleInstance, true)
                if (!data.setHeight)
                    scrollview.flickableItem.contentHeight = scrollview.flickableItem.height * 4
            }

            //The final "1" is a workaround to bug #1549256
            flick(scrollview.flickableItem, 20, 20, -units.gu(10), -units.gu(10), undefined, undefined, undefined, undefined, 1)
            tryCompare(scrollview.flickableItem, "moving", true, 5000, "Flickable not moving after simulating a flick.")

            //NOTE: this becomes FLAKY if you move the mouse inside the window the test is being rendered on
            //because the mouse moves interfere with the flicking
            tryCompare(horizontalScrollbar.__styleInstance, "state", "thumb", 5000, "Horizontal scrollbar: wrong style while flicking.")
            tryCompare(verticalScrollbar.__styleInstance, "state", "thumb", 5000, "Vertical scrollbar: wrong style while flicking.")

            //we don't set it up before because the hinting feature already changes the style to thumb
            //at the beginning
            setupSignalSpy(anotherSignalSpy, horizontalScrollbar.__styleInstance, "stateChanged")
            setupSignalSpy(ultimateSignalSpy, verticalScrollbar.__styleInstance, "stateChanged")

            //wait for the flickable to stop
            tryCompare(scrollview.flickableItem, "moving", false, 5000, "Flickable still moving after timeout.")

            compare(horizontalScrollbar.__styleInstance.state, "hidden", "Horizontal scrollbar: wrong style after the item stopped moving.")
            compare(verticalScrollbar.__styleInstance.state, "hidden", "Vertical scrollbar: wrong style after the item stopped moving.")
        }

        function test_keyEventsPropagation() {
            var freshTestItem = getFreshScrollView()
            var scrollview = freshTestItem.scrollview
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var verticalScrollbar = getVerticalScrollbar(scrollview)

            //focus the scrollview
            focusScrollView(scrollview)
            compare(scrollview.Keys.enabled, true, "Key events propagation: scrollview key handling not enabled.")

            setupSignalSpy(signalSpy, scrollview.Keys, "onPressed")
            setupSignalSpy(anotherSignalSpy, freshTestItem.Keys, "onPressed")

            keyClick(Qt.Key_Home)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: scrollview did not receive the key event.")

            keyClick(Qt.Key_End)
            signalSpy.wait()
            compare(signalSpy.count, 2, "Key events propagation: scrollview did not receive the key event.")

            keyClick(Qt.Key_PageDown)
            signalSpy.wait()
            compare(signalSpy.count, 3, "Key events propagation: scrollview did not receive the key event.")

            keyClick(Qt.Key_PageUp)
            signalSpy.wait()
            compare(signalSpy.count, 4, "Key events propagation: scrollview did not receive the key event.")

            setupSignalSpy(signalSpy, scrollview.Keys, "onLeftPressed")
            keyClick(Qt.Key_Left)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: scrollview did not receive the left key event.")

            setupSignalSpy(signalSpy, scrollview.Keys, "onRightPressed")
            keyClick(Qt.Key_Right)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: scrollview did not receive the right key event.")

            setupSignalSpy(signalSpy, scrollview.Keys, "onUpPressed")
            keyClick(Qt.Key_Up)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: scrollview did not receive the up key event.")

            setupSignalSpy(signalSpy, scrollview.Keys, "onDownPressed")
            keyClick(Qt.Key_Down)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: scrollview did not receive the down key event.")

            //Check that the parent has not received any event so far.
            compare(anotherSignalSpy.count, 0, "Key events propagation: parent should have not received any event.")

            //Fire random keys and make sure we're letting the rest of the keys propagate to the parent.
            keyClick(Qt.Key_A)
            anotherSignalSpy.wait()
            keyClick(Qt.Key_N)
            anotherSignalSpy.wait()
            keyClick(Qt.Key_P)
            anotherSignalSpy.wait()
            keyClick(Qt.Key_Return)
            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 4, "Key events propagation: parent did not receive the right number of key events.")
        }

        function test_arrowKeys_scrollableView_data() {
            return [
                        {tag: "Scrollable", addContentMargins: false},
                        {tag: "ScrollableWithContentMargins", addContentMargins: true},
                    ]
        }

        function test_arrowKeys_scrollableView(data) {
            var testComp = scrollViewComp.createObject(column)
            verify(testComp !== null, "Error: dynamic item creation failed.")

            //make sure cleanup() deletes the dynamic object if a test fails
            currComponent = testComp

            var scrollview = testComp.scrollview
            var flickable = testComp.scrollview.flickableItem
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var verticalScrollbar = getVerticalScrollbar(scrollview)
            var horizontalScrollbarStyle = horizontalScrollbar.__styleInstance
            var verticalScrollbarStyle = verticalScrollbar.__styleInstance

            if (data.addContentMargins) {
                addContentMargins(flickable)
            }

            //doublecheck that we have enough room to make at least
            //a long scroll (like pgUp/pgDown) in both directions
            //This is an assumption of the coming tests
            verify(flickable.contentWidth + flickable.leftMargin + flickable.rightMargin
                   > flickable.width * horizontalScrollbarStyle.longScrollingRatio,
                   "Content width must allow a long scrolling action.")
            verify(flickable.contentHeight + flickable.topMargin + flickable.bottomMargin
                   > flickable.height * horizontalScrollbarStyle.longScrollingRatio,
                   "Content height must allow a long scrolling action.")

            focusScrollView(testComp.scrollview)
            setContentPositionToTopLeft(flickable)
            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY

            //************ITEM POSITIONED AT THE TOP LEFT CORNER*************
            //(Left/Up/PageUp/Home should not have any effect)
            sendKeyAndCheckItHasNoEffect("Left", Qt.Key_Left, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("Up", Qt.Key_Up, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("PageUp", Qt.Key_PageUp, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("Home", Qt.Key_Home, flickable, contentXBackup, contentYBackup)

            //Key_Right should work as intended when the item is at the top left
            keyClick(Qt.Key_Right)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           horizontalScrollbarStyle, false, 1, "Right key")

            //reset the position of the content and test another key
            setContentPositionToTopLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            //Key_Down should work as intended
            keyClick(Qt.Key_Down)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           verticalScrollbarStyle, false, 1, "Down key")

            //reset the position of the content and test another key
            setContentPositionToTopLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY
            keyClick(Qt.Key_PageDown)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           verticalScrollbarStyle, true, 1, "PageDown key")

            //reset the position of the content and test another key
            setContentPositionToTopLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY
            keyClick(Qt.Key_End)
            //check that Key_End scrolls to the bottom
            var newContentY = flickable.contentHeight + flickable.bottomMargin - flickable.height
            tryCompare(flickable, "contentY", newContentY, 5000, "End key handling: wrong contentY.")
            wait(150)
            compare(flickable.contentY, newContentY, "End key handling: wrong contentY.")
            compare(flickable.contentX, contentXBackup, "End key must not change contentX.")


            //************ITEM POSITIONED AT THE TOP RIGHT CORNER*************
            //(Right/Up/PageUp/Home should have no effect)

            //Note: leftMargin is not considered here, because when you have leftMargin != 0,
            //contentX = 0 aligns to the beginning of the content, not to the beginning of leftMargin.
            //This also means that aligning content to the left means setting contentX = -leftMargin
            flickable.contentY = -flickable.topMargin
            flickable.contentX = flickable.contentWidth + flickable.rightMargin - flickable.width
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            sendKeyAndCheckItHasNoEffect("Right", Qt.Key_Right, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("Up", Qt.Key_Up, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("PageUp", Qt.Key_PageUp, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("Home", Qt.Key_Home, flickable, contentXBackup, contentYBackup)

            //Key_Left should work as intended
            keyClick(Qt.Key_Left)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           horizontalScrollbarStyle, false, -1, "Left key")

            //We don't retest PgDown/End/Down

            //************ITEM POSITIONED AT THE BOTTOM LEFT CORNER*************
            //Left/Down/PageDown/End should have no effect
            setContentPositionToBottomLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY
            compare(flickable.contentX, -flickable.leftMargin, "Item not positioned at the bottom left corner.")

            sendKeyAndCheckItHasNoEffect("Left", Qt.Key_Left, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("Down", Qt.Key_Down, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("PageDown", Qt.Key_PageDown, flickable, contentXBackup, contentYBackup)
            sendKeyAndCheckItHasNoEffect("End", Qt.Key_End, flickable, contentXBackup, contentYBackup)

            //Key_Up instead should work as intended
            keyClick(Qt.Key_Up)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           verticalScrollbarStyle, false, -1, "Up key")

            //Reposition at the bottom left corner and test PageUp
            setContentPositionToBottomLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY
            keyClick(Qt.Key_PageUp)
            checkScrolling(flickable, contentXBackup, contentYBackup,
                           verticalScrollbarStyle, true, -1, "PageUp key")

            setContentPositionToBottomLeft(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY
            newContentY = -flickable.topMargin
            keyClick(Qt.Key_Home)
            tryCompare(flickable, "contentY", newContentY, 5000, "Home key handling: wrong contentY.")
            wait(150)
            compare(flickable.contentY, newContentY, "Home key handling: wrong contentY.")
            compare(flickable.contentX, contentXBackup, "Home key must not change contentX.")
        }

        function test_arrowKeys_nonScrollableView_data() {
            return [
                        {tag: "NonScrollable", addContentMargins: false},
                        {tag: "NonScrollableWithContentMargins", addContentMargins: true}
                    ]
        }

        function test_arrowKeys_nonScrollableView(data) {
            var testComp = nonScrollableTestViewComp.createObject(column)
            verify(testComp !== null, "Successfully created test object.")

            //make sure cleanup() deletes the dynamic object if a test fails
            currComponent = testComp

            var scrollview = testComp.scrollview
            var flickable = testComp.scrollview.flickableItem
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var style = horizontalScrollbar.__styleInstance

            if (data.addContentMargins) {
                addContentMargins(flickable)
            }

            var oldContentX = flickable.contentX
            var oldContentY = flickable.contentY

            //make sure the content is not scrollable
            verify(flickable.contentWidth + flickable.leftMargin + flickable.rightMargin
                   < flickable.width, "The content must be non scrollable.")
            verify(flickable.contentHeight + flickable.topMargin + flickable.bottomMargin
                   < flickable.height, "The content must be non scrollable.")
            compare(style.isScrollable, false, "Scrollbar thinks the content is scrollable when it is not.")

            sendKeyAndCheckItHasNoEffect("Right", Qt.Key_Right, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("Left", Qt.Key_Left, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("Up", Qt.Key_Up, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("Down", Qt.Key_Down, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("PageUp", Qt.Key_PageUp, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("PageDown", Qt.Key_PageDown, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("Home", Qt.Key_Home, flickable, oldContentX, oldContentY)
            sendKeyAndCheckItHasNoEffect("End", Qt.Key_End, flickable, oldContentX, oldContentY)
        }

        function test_escKey_data() {
            return [
                        { tag: "Vertical scrollbar", vertical: true },
                        { tag: "Horizontal scrollbar", vertical: false }
                    ]
        }

        function test_escKey(data) {
            var firstTestItem = getFreshScrollView()
            var scrollview = firstTestItem.scrollview
            var flickable = scrollview.flickableItem
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var verticalScrollbar = getVerticalScrollbar(scrollview)
            var style = horizontalScrollbar.__styleInstance

            focusScrollView(scrollview)
            triggerSteppersMode(data.vertical ? verticalScrollbar : horizontalScrollbar)

            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY

            dragThumbForwardAndCheckPos_noRelease(
                        (data.vertical ? verticalScrollbar : horizontalScrollbar),
                        flickable,
                        units.gu(5), units.gu(10))

            setupSignalSpy(signalSpy, scrollview.Keys, "onPressed")
            keyClick(Qt.Key_Escape)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Wrong number of signals fired after pressing Esc.")

            checkNoContentPositionChange("Escape key", flickable, contentXBackup, contentYBackup)

            mouseRelease(style.isVertical ? getThumb(verticalScrollbar) : getThumb(horizontalScrollbar),
                                            (style.isVertical ? 0 : units.gu(12)),
                                            (style.isVertical ? units.gu(12) : 0))
        }

        function test_noFlickeringWhenUsingArrowKeysWhileDraggingThumb_data() {
            return [
                        { tag: "Vertical scrollbar", vertical: true },
                        { tag: "Horizontal scrollbar", vertical: false }
                    ]
        }
        //Test that any running scrolling animation is stopped when the user drags the thumb
        //to avoid conflicts and flickering
        function test_noFlickeringWhenUsingArrowKeysWhileDraggingThumb(data) {
            var firstTestItem = getFreshScrollView()
            var scrollview = firstTestItem.scrollview
            var flickable = scrollview.flickableItem
            var scrollbar = data.vertical ? getVerticalScrollbar(scrollview)
                                          : getHorizontalScrollbar(scrollview)
            var style = scrollbar.__styleInstance
            var scrollAnimation = getScrollAnimation(scrollbar)
            var thumb = getThumb(scrollbar)

            focusScrollView(scrollview)
            triggerSteppersMode(scrollbar)

            dragThumbForwardAndCheckPos_noRelease(scrollbar, flickable, units.gu(5), units.gu(10))

            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY

            setupSignalSpy(signalSpy, scrollview.Keys, data.vertical ? "onDownPressed" : "onRightPressed")
            setupSignalSpy(anotherSignalSpy, scrollAnimation, "onRunningChanged")
            keyClick(data.vertical ? Qt.Key_Down : Qt.Key_Right)

            signalSpy.wait()
            compare(signalSpy.count, 1, "Wrong number of signals fired after pressing Esc.")

            anotherSignalSpy.wait()
            compare(scrollAnimation.running, true, "Scroll animation not running after using an arrow key.")

            //Move once again, this time while the animation is running
            setupSignalSpy(signalSpy, thumb, style.isVertical ? "onYChanged" : "onXChanged")
            mouseMove(thumb,
                      (style.isVertical ? 0 : units.gu(11)),
                      (style.isVertical ? units.gu(11) : 0))
            signalSpy.wait()

            compare(scrollAnimation.running, false, "Scroll animation must stop after dragging the thumb.")

            mouseRelease(thumb,
                         (style.isVertical ? 0 : units.gu(12)),
                         (style.isVertical ? units.gu(12) : 0))
        }

        function test_scrollbarsPositioning_data() {
            return [
                        { tag: "Not scrollable", scrollableHorizontally: false, scrollableVertically: false },
                        { tag: "Horizontally scrollable", scrollableHorizontally: true, scrollableVertically: false } ,
                        { tag: "Vertically scrollable", scrollableHorizontally: false, scrollableVertically: true },
                        { tag: "Both scrollable", scrollableHorizontally: true, scrollableVertically: true }
                    ]
        }

        function test_scrollbarsPositioning(data) {
            var firstTestItem = getFreshScrollView()
            var scrollview = firstTestItem.scrollview
            var flickable = scrollview.flickableItem
            var horizontalScrollbar = getHorizontalScrollbar(scrollview)
            var verticalScrollbar = getVerticalScrollbar(scrollview)
            var horStyle = horizontalScrollbar.__styleInstance
            var verStyle = verticalScrollbar.__styleInstance
            console.log(verticalScrollbar)

            var viewport = scrollview.viewport

            //make it not scrollable
            flickable.contentHeight = flickable.height
            flickable.contentWidth = flickable.width

            tryCompare(horStyle, "isScrollable", false)
            tryCompare(verStyle, "isScrollable", false)

            if (data.scrollableHorizontally) {
                flickable.contentWidth = flickable.width + 1
                tryCompare(horStyle, "isScrollable", true)
            }
            if (data.scrollableVertically) {
                flickable.contentHeight = flickable.height + 1
                tryCompare(verStyle, "isScrollable", true)

            }

            compare(verticalScrollbar.anchors.top, viewport.anchors.top,
                    "Horizontal scrollbar: wrong top anchors.")
            compare(verticalScrollbar.anchors.bottom, viewport.anchors.bottom,
                    "Horizontal scrollbar: wrong bottom anchors.")

            //TODO: TEST THAT LEFT ANCHOR IS UNDEFINED. THIS REQUIRES A CUSTOM C++ METHOD
            //THAT WE CURRENTLY DO NOT HAVE

            compare(verticalScrollbar.anchors.right, viewport.anchors.right,
                    "Horizontal scrollbar: wrong right anchors.")
            compare(verticalScrollbar.anchors.topMargin, 0,
                    "Horizontal scrollbar: wrong topMargin.")
            compare(verticalScrollbar.anchors.rightMargin, 0,
                    "Horizontal scrollbar: wrong rightMargin.")


            //TODO: TEST THAT TOP ANCHOR IS UNDEFINED. THIS REQUIRES A CUSTOM C++ METHOD
            //THAT WE CURRENTLY DO NOT HAVE

            compare(horizontalScrollbar.anchors.bottom, viewport.anchors.bottom,
                    "Horizontal scrollbar: wrong bottom anchors.")
            compare(horizontalScrollbar.anchors.left, viewport.anchors.left,
                    "Horizontal scrollbar: wrong left anchors.")
            compare(horizontalScrollbar.anchors.right, viewport.anchors.right,
                    "Horizontal scrollbar: wrong right anchors.")
            compare(horizontalScrollbar.anchors.bottomMargin, 0,
                    "Horizontal scrollbar: wrong bottomMargin.")
            compare(horizontalScrollbar.anchors.leftMargin, 0,
                    "Horizontal scrollbar: wrong leftMargin.")

            //check they're not overlapping
            if (data.scrollableHorizontally && data.scrollableVertically) {
                compare(verticalScrollbar.anchors.bottomMargin,
                        horStyle.troughThicknessSteppersStyle,
                        "Vertical scrollbar: wrong bottom margin.")

                compare(horizontalScrollbar.anchors.rightMargin,
                        verStyle.troughThicknessSteppersStyle,
                        "Horizontal scrollbar: wrong right margin.")
            }
        }

        function test_transferFocusFromOneScrollViewToAnother() {
            var firstTestItem = getFreshScrollView()
            var secondTestItem = getFreshScrollView()
            var firstScrollView = firstTestItem.scrollview
            var secondScrollView = secondTestItem.scrollview
            //make sure cleanup() deletes the dynamic object if a test fails
            currComponent = firstTestItem
            anotherDynamicComp = secondTestItem

            //wait until the column correctly lays out the new scrollview
            waitForRendering(secondTestItem)

            setupSignalSpy(signalSpy, firstScrollView.Keys, "onDownPressed")
            setupSignalSpy(anotherSignalSpy, secondScrollView.Keys, "onDownPressed")

            //focus the scrollview
            focusScrollView(firstScrollView)

            //check that the other one is not in focus
            compare(secondScrollView.focus, false, "Key events propagation: second scrollview should not be focused.")
            compare(secondScrollView.activeFocus, false, "Key events propagation: second scrollview should not be focused.")

            keyClick(Qt.Key_Down)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: first scrollview did not receive the down key event.")
            compare(anotherSignalSpy.count, 0, "Key events propagation: key event delivered to the wrong scrollview.")

            focusScrollView(secondScrollView)

            //check that the other one is now not in focus
            compare(firstScrollView.focus, false, "Key events propagation: scrollview still focused.")
            compare(firstScrollView.activeFocus, false, "Key events propagation: scrollview still focused.")

            keyClick(Qt.Key_Down)
            anotherSignalSpy.wait()
            compare(signalSpy.count, 1, "Key events propagation: key event delivered to the wrong scrollview.")
            compare(anotherSignalSpy.count, 1, "Key events propagation: second scrollview did not receive the key event.")
        }

        function test_transferFocusToAndFromInnerScrollView() {
            var outerScrollViewComp = nestedScrollViewComp.createObject(column)
            verify(outerScrollViewComp !== null, "Error: dynamic item creation failed.")
            currComponent = outerScrollViewComp

            var outerScrollView = outerScrollViewComp.scrollview
            var innerScrollView = outerScrollViewComp.innerScrollView

            focusScrollView(innerScrollView)
            //ScrollView will get activeFocus as well because it is a FocusScope
            compare(outerScrollView.focus, true, "Outer ScrollView focus should be true.")
            compare(outerScrollView.activeFocus, true, "Outer ScrollView did not get active focus.")

            mouseClick(innerScrollView, innerScrollView.width + 1, 0)
            expectFail("", "FIXME: nested ScrollView doesn't lose focus.")
            compare(innerScrollView.activeFocus, false, "Inner ScrollView focus is still focused after clicking on the outer ScrollView.")
            compare(outerScrollView.activeFocus, true, "Outer ScrollView does not have activeFocus after clicking on the outer ScrollView.")
        }

        //Test that a child can override key events that would otherwise be handled by ScrollView
        //AND
        //Test that key events not consumed by the children will be handled by the parent ScrollView
        function test_childCanOverrideKeyHandling() {
            var outerScrollViewComp = getFreshScrollView()
            var outerScrollView = outerScrollViewComp.scrollview
            var innerItem = outerScrollViewComp.content

            outerScrollViewComp.innerItemConsumesKeyEvents = true

            compare(innerItem.Keys.enabled, true, "Inner item does not handle key events.")

            mouseClick(innerItem, 0, 0)
            compare(innerItem.focus, true, "Inner item does not have focus.")
            compare(innerItem.activeFocus, true, "Inner item does not have active focus.")

            setupSignalSpy(signalSpy, innerItem.Keys, "onPressed")
            setupSignalSpy(anotherSignalSpy, outerScrollView.Keys, "onPressed")

            //PageDown should be handled by the child
            keyClick(Qt.Key_PageDown)
            signalSpy.wait()
            compare(signalSpy.count, 1, "Inner item failed to override key handling.")
            compare(anotherSignalSpy.count, 0, "Outer ScrollView received a key event.")

            //PageUp should be handled by the outer ScrollView because the child doesn't handle it
            keyClick(Qt.Key_PageUp)
            signalSpy.wait()
            anotherSignalSpy.wait()
            compare(signalSpy.count, 2, "Inner item is not propagating key events as expected.")
            compare(anotherSignalSpy.count, 1, "Outer ScrollView did received the key event.")
        }
    }
}
