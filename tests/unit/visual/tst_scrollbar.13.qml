/*
 * Copyright 2015-2016 Canonical Ltd.
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
import QtTest 1.1
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)


    Component {
        id: flickableComp
        Item {
            id: item
            width: units.gu(20)
            height: units.gu(30)
            property alias flickable: freshFlickable
            property alias scrollbar: freshScrollbar
            property alias content: content
            property alias scrollbarAlignment: freshScrollbar.align

            //Don't change this to a ListView, this usecase has to be simple,
            //we don't want the difficulties added by ListView, i.e. delegates
            //size estimation, dynamic contentWidth/Height, etc
            Flickable {
                id: freshFlickable
                anchors.fill: parent
                contentHeight: content.height
                contentWidth: content.width
                clip: true
                Rectangle {
                    id: content
                    width: units.gu(40)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    color: "blue"
                }
            }
            Scrollbar {
                id: freshScrollbar
                flickableItem: parent.flickable
            }
        }
    }

    Component {
        id: freshScrollbar
        Scrollbar { }
    }

    SignalSpy {
        id: signalSpy
    }
    SignalSpy {
        id: anotherSignalSpy
    }

    Flickable {
        anchors.fill: parent
        Column {
            id: column
            Repeater {
                model: layoutsModel
            }
        }
    }

    VisualItemModel {
        id: layoutsModel
        Item {
            width: childrenRect.width
            height: childrenRect.height
            Flickable {
                id: flickable_bottomAlign_anchors
            }
            Scrollbar {
                id: scrollbar_bottomAlign_anchors
                flickableItem: flickable_bottomAlign_anchors
                align: Qt.AlignBottom
            }

            Flickable {
                id: flickable_topAlign_anchors
            }
            Scrollbar {
                id: scrollbar_topAlign_anchors
                flickableItem: flickable_topAlign_anchors
                align: Qt.AlignBottom
            }

            Flickable {
                id: flickable_leadingAlign_anchors
            }
            Scrollbar {
                id: scrollbar_leadingAlign_anchors
                flickableItem: flickable_leadingAlign_anchors
                align: Qt.AlignLeading
            }

            Flickable {
                id: flickable_trailingAlign_anchors
            }
            Scrollbar {
                id: scrollbar_trailingAlign_anchors
                flickableItem: flickable_trailingAlign_anchors
                align: Qt.AlignTrailing
            }

            Flickable {
                id: defaultValuesFlickable
            }
            Scrollbar {
                id: defaultValuesScrollbar
                flickableItem: defaultValuesFlickable
            }

            //Items used for values sanity checks
            Flickable {
                id: flickableSanityCheck
                width: units.gu(30)
                height: units.gu(40)
                Rectangle {
                    id: contentSanityCheck
                    width: units.gu(20)
                    height: units.gu(30)
                    color: "yellow"
                }
            }
            Scrollbar {
                id: scrollbarSanityCheck
                flickableItem: flickableSanityCheck
            }
        }
    }

    ScrollbarTestCase13 {
        name: "Scrollbar"

        function getFreshFlickable(alignment) {
            var wrapper = createTemporaryObject(flickableComp, column, { scrollbarAlignment: alignment } )
            verify(wrapper !== null, "Error: dynamic item creation failed.")
            compare(wrapper.scrollbar.align, alignment, "getFreshFlickable: wrong alignment.")
            return wrapper
        }

        function init_data() {
            return [
                        { tag: "vertical scrollbar", alignment: Qt.AlignTrailing },
                        { tag: "horizontal scrollbar", alignment: Qt.AlignBottom }
                    ]
        }

        function clickOnStepperAndCheckNoContentPositionChange(msgPrefix, itemToClickOn, flickable, expectedContentX, expectedContentY) {
            clickInTheMiddle(itemToClickOn)
            wait(150)
            checkNoContentPositionChange(msgPrefix, flickable, expectedContentX, expectedContentY)
        }

        //NOTE: this does not test that the properties have the correct value, just that
        //their values is not out of range (so it only applies to numeric values or enums)
        function performStyleSanityCheck(scrollbar) {
            var style = scrollbar.__styleInstance
            /*****************************************************
             *      STYLING PROPERTIES                           *
             *****************************************************/
            verify(style.minimumSliderSize >= 0, "Sanity check: invalid property value.")
            verify(style.overlayOpacityWhenShown > 0, "Sanity check: invalid property value.")
            verify(style.overlayOpacityWhenHidden >= 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessSteppersStyle > 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessThumbStyle > 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessIndicatorStyle > 0, "Sanity check: invalid property value.")
            verify(style.sliderRadius >= 0, "Sanity check: invalid property value.")
            verify(style.thumbThickness > 0, "Sanity check: invalid property value.")
            verify(style.indicatorThickness > 0, "Sanity check: invalid property value.")
            verify(style.scrollbarThicknessAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeInAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeOutAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeOutPause >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarCollapsePause >= 0, "Sanity check: invalid property value.")
            verify(style.shortScrollingRatio > 0, "Sanity check: invalid property value.")
            verify(style.longScrollingRatio > 0, "Sanity check: invalid property value.")
            verify(style.hintingStyle === 'thumb'
                   || style.hintingStyle === 'indicator', "Sanity check: invalid property value.")
            verify(style.thumbsExtremesMargin >= 0, "Sanity check: invalid property value.")

            /*****************************************************
             *      HELPER PROPERTIES                            *
             *****************************************************/
            verify(!!style.thumb, "Sanity check: invalid property value.")
            verify(!!style.trough, "Sanity check: invalid property value.")
            //flickable helper properties
            verify(!!style.flickableItem, "Sanity check: invalid property value.")

            /*****************************************************
             *      INTERNAL PROPERTIES AND FUNCTIONS            *
             *****************************************************/
            verify(style.nonOverlayScrollbarMargin >= 0, "Sanity check: invalid property value.")
            verify(style.touchDragStartMargin >= 0, "Sanity check: invalid property value.")
            verify(style.dragThreshold >= 0, "Sanity check: invalid property value.")
            verify(style.__stepperAssetWidth <= style.troughThicknessSteppersStyle, "Sanity check: check that the stepper arrow is narrower than the trough.")
            verify(style.__stepperImgOpacityNormal < style.__stepperImgOpacityOnHover, "Sanity check: check that stepper is darker on hover than on normal state.")
            verify(style.__stepperImgOpacityOnHover < style.__stepperImgOpacityOnPressed, "Sanity check: check that stepper is darker on pressed than on hover state.")
            verify(style.__stepperBgOpacityOnHover < style.__stepperBgOpacityOnPressed, "Sanity check: check that stepper background is darker on pressed than on hover state.")
        }

        function test_visibility(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var scrollbar = freshTestItem.scrollbar
            compare(scrollbar.visible, true, "Wrong Scrollbar visible property value")
            
            scrollbar.flickableItem = null
            compare(scrollbar.visible, false, "Wrong Scrollbar visible property value")

            //check that the same apply to a scrollbar that was born without Flickable
            var nullFlickableScrollbar = freshScrollbar.createObject(freshTestItem)
            verify(nullFlickableScrollbar !== null, "Error: dynamic item creation failed.")
            compare(nullFlickableScrollbar.visible, false, "Scrollbar is visible before an initialized flickable is assigned to it")

            nullFlickableScrollbar.destroy()
        }

        //no need to test the anchors values when flickable is not set because we already
        //test that the scrollbar is hidden when there's no flickable set
        function test_bottomAlign_anchors() {
            compare(scrollbar_bottomAlign_anchors.flickableItem,
                    flickable_bottomAlign_anchors, "wrong flickableItem")
            compare(scrollbar_bottomAlign_anchors.anchors.left,
                    scrollbar_bottomAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.right,
                    scrollbar_bottomAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.bottom,
                    scrollbar_bottomAlign_anchors.flickableItem.bottom, "bottom anchor")
            //we can't check that the top anchor is not anchored
        }
        function test_topAlign_anchors() {
            compare(scrollbar_topAlign_anchors.flickableItem,
                    flickable_topAlign_anchors, "wrong flickableItem")
            compare(scrollbar_topAlign_anchors.anchors.left,
                    scrollbar_topAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_topAlign_anchors.anchors.right,
                    scrollbar_topAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_topAlign_anchors.anchors.top,
                    scrollbar_topAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_leadingAlign_anchors() {
            compare(scrollbar_leadingAlign_anchors.flickableItem,
                    flickable_leadingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_leadingAlign_anchors.anchors.left,
                    scrollbar_leadingAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.bottom,
                    scrollbar_leadingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.top,
                    scrollbar_leadingAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_trailingAlign_anchors() {
            compare(scrollbar_trailingAlign_anchors.flickableItem,
                    flickable_trailingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_trailingAlign_anchors.anchors.right,
                    scrollbar_trailingAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.bottom,
                    scrollbar_trailingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.top,
                    scrollbar_trailingAlign_anchors.flickableItem.top, "top anchor")
        }

        function test_indicatorStyleWhileFlicking_shortContent(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            compare(scrollbar.__styleInstance.veryLongContentItem, false, "Scrollable item should be short.")

            //The final "1" is a workaround to bug #1549256
            flick(flickable, 2, 2, -units.gu(10), -units.gu(10), undefined, undefined, undefined, undefined, 1)

            tryCompare(flickable, "moving", true, 5000, "Flickable not moving after simulating a flick.")

            compare(scrollbar.__styleInstance.state, "indicator", "Wrong style while flicking.")

            //we don't set it up before because the hinting feature already changes the state
            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")

            //wait for the flickable to stop
            tryCompare(flickable, "moving", false, 5000, "Flickable still moving after timeout.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after the item stopped moving.")
        }

        function test_thumbStyleWhileFlicking_veryLongContent(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            setupSignalSpy(signalSpy, scrollbar.__styleInstance, "veryLongContentItemChanged")
            setVeryLongContentItem(flickable, scrollbar.__styleInstance, false)
            signalSpy.wait()

            compare(scrollbar.__styleInstance.veryLongContentItem, true, "Very long content item not detected")

            //The final "1" is a workaround to bug #1549256
            flick(flickable, 2, 2, -units.gu(10), -units.gu(10), undefined, undefined, undefined, undefined, 1)

            tryCompare(flickable, "moving", true, 5000, "Flickable not moving after simulating a flick.")

            compare(scrollbar.__styleInstance.state, "thumb", "Wrong style while flicking a very long item")

            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")
            //wait for the flickable to stop
            tryCompare(flickable, "moving", false, 5000, "Flickable still moving after timeout.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after a the item stopped moving.")
        }

        function test_scrollingWithContentMargins(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = freshTestItem.scrollbar.__styleInstance
            var scrollbarUtils = getScrollbarUtils(scrollbar)
            //to make sure that it's only scrolling in the direction it should
            var notScrollingProperty = scrollbarUtils.propContent === "contentX" ? "contentY" : "contentX"
            addContentMargins(flickable)

            style.scrollToBeginning(false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical ? -flickable.topMargin : -flickable.leftMargin,
                                       "Wrong beginning contentY")

            style.scrollToEnd(false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical
                    ? flickable.contentHeight + flickable.bottomMargin - flickable[scrollbarUtils.propSize]
                    : flickable.contentWidth + flickable.rightMargin - flickable[scrollbarUtils.propSize],
                      "Wrong end contentY")

            //contentHeight+top+bottom is the max you can scroll anyway, so it's
            //safe to use it, and the scrollbar should go to returnToBounds() if it goes beyond
            var maxScrolling = style.totalContentSize
            style.scrollBy(-maxScrolling, false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical ? -flickable.topMargin : -flickable.leftMargin,
                                       "scrollBy: wrong contentY after scrolling to the beginning")

            style.scrollBy(maxScrolling, false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical
                    ? flickable.contentHeight + flickable.bottomMargin - flickable[scrollbarUtils.propSize]
                    : flickable.contentWidth + flickable.rightMargin - flickable[scrollbarUtils.propSize],
                      "scrollBy: wrong contentY after scrolling to the end")

            //we can only scroll up now
            var oldContentProp = flickable[scrollbarUtils.propContent]
            style.scrollBy(-units.gu(1), false)
            compare(flickable[scrollbarUtils.propContent],
                    oldContentProp - units.gu(1),
                    "scrollBy: wrong contentY after scrolling by +1GU")
            style.scrollBy(units.gu(1), false)
            compare(flickable[scrollbarUtils.propContent],
                    oldContentProp,
                    "scrollBy: wrong contentY after scrolling by -1GU")
        }

        function test_showSteppers(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = scrollbar.__styleInstance
            var steppersTransition = findInListProperty(scrollbar.__styleInstance.transitions, "indicatorToThumbSteppersTransition")
            verify(steppersTransition !== null, "Could not find transition object")

            setupSignalSpy(signalSpy, scrollbar.__styleInstance, "stateChanged")
            setupSignalSpy(anotherSignalSpy, steppersTransition, "runningChanged")

            //show steppers on mouse move
            mouseMove(scrollbar, 0, 0)

            signalSpy.wait()
            compare(signalSpy.count, 1, "Mouse move doesn't trigger state change")
            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "Mouse move doesn't trigger state transition")

            compare(scrollbar.__styleInstance.state, "steppers", "Mouse move doesn't trigger steppers state.")
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var trough = getTrough(scrollbar)

            compare(trough.visible, true, "Trough not visible in steppers state")
            compare(firstStepper.visible, true, "First stepper not visible in steppers state")
            compare(secondStepper.visible, true, "Second stepper not visible in steppers state")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 2, "State transition does not complete.")
            compare(steppersTransition.running, false, "State transition does not stop.")

            if (style.isVertical) {
                compare(trough.width, scrollbar.__styleInstance.troughThicknessSteppersStyle, "Wrong trough thickness in steppers style")
            } else {
                compare(trough.height, scrollbar.__styleInstance.troughThicknessSteppersStyle, "Wrong trough thickness in steppers style")
            }
        }

        function checkSteppersColour(scrollbar, firstStepperStateString, secondStepperStateString, testDescription) {
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var firstStepperIcon = getFirstStepperIcon(scrollbar)
            var secondStepperIcon = getSecondStepperIcon(scrollbar)
            var style = scrollbar.__styleInstance

            var stepperBgColorBase = style.stepperBgColor
            var stepperImgColor = style.sliderColor

            var stepperBgColorDisabled = "transparent"
            var stepperImgColorDisabled = Qt.rgba(stepperImgColor.r, stepperImgColor.g, stepperImgColor.b,
                                                  stepperImgColor.a * style.__stepperImgOpacityDisabled)
            var stepperBgColorNormal = "transparent"
            var stepperImgColorNormal = Qt.rgba(stepperImgColor.r, stepperImgColor.g, stepperImgColor.b,
                                                stepperImgColor.a * style.__stepperImgOpacityNormal)
            var stepperBgColorOnHover = Qt.rgba(stepperBgColorBase.r, stepperBgColorBase.g, stepperBgColorBase.b,
                                                stepperBgColorBase.a *  style.__stepperBgOpacityOnHover)
            var stepperImgColorOnHover = Qt.rgba(stepperImgColor.r, stepperImgColor.g, stepperImgColor.b,
                                                 stepperImgColor.a * style.__stepperImgOpacityOnHover)
            var stepperBgColorOnPressed = Qt.rgba(stepperBgColorBase.r, stepperBgColorBase.g, stepperBgColorBase.b,
                                                  stepperBgColorBase.a *  style.__stepperBgOpacityOnPressed)
            var stepperImgColorOnPressed = Qt.rgba(stepperImgColor.r, stepperImgColor.g, stepperImgColor.b,
                                                   stepperImgColor.a * style.__stepperImgOpacityOnPressed)

            switch (firstStepperStateString) {
            case "disabled":
                var expectedFirstStepperColour = stepperBgColorDisabled
                var expectedFirstStepperIconColour = stepperImgColorDisabled
                break
            case "normal":
                var expectedFirstStepperColour = stepperBgColorNormal
                var expectedFirstStepperIconColour = stepperImgColorNormal
                break
            case "hovered":
                var expectedFirstStepperColour = stepperBgColorOnHover
                var expectedFirstStepperIconColour = stepperImgColorOnHover
                break
            case "pressed":
                var expectedFirstStepperColour = stepperBgColorOnPressed
                var expectedFirstStepperIconColour = stepperImgColorOnPressed
                break
            default:
                fail("Invalid stepper state string")
            }

            switch (secondStepperStateString) {
            case "disabled":
                var expectedSecondStepperColour = stepperBgColorDisabled
                var expectedSecondStepperIconColour = stepperImgColorDisabled
                break
            case "normal":
                var expectedSecondStepperColour = stepperBgColorNormal
                var expectedSecondStepperIconColour = stepperImgColorNormal
                break
            case "hovered":
                var expectedSecondStepperColour = stepperBgColorOnHover
                var expectedSecondStepperIconColour = stepperImgColorOnHover
                break
            case "pressed":
                var expectedSecondStepperColour = stepperBgColorOnPressed
                var expectedSecondStepperIconColour = stepperImgColorOnPressed
                break
            default:
                fail("Invalid stepper state string")
            }

            var msg = "First stepper expected state: " + firstStepperStateString + "."
                    + " Second stepper expected state: " + secondStepperStateString + ". "
                    + " Test description: " + testDescription + ". "
                    + " Error encountered: "

            //compare() returns pass on different colours, see https://bugreports.qt.io/browse/QTBUG-34878
            compare(Qt.colorEqual(firstStepper.color, expectedFirstStepperColour), true,
                    msg + "wrong first stepper background color.")
            compare(Qt.colorEqual(firstStepperIcon.color, expectedFirstStepperIconColour), true,
                    msg + "wrong first stepper icon color.")
            compare(Qt.colorEqual(secondStepper.color, expectedSecondStepperColour), true,
                    msg + "wrong second stepper background color.")
            compare(Qt.colorEqual(secondStepperIcon.color, expectedSecondStepperIconColour), true,
                    msg + "wrong second stepper icon color.")
        }

        function test_checkStepperStatesStyling(data) {
            skip("Scrollbar already in steppers mode under Qt 5.9, causing failure.")
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = scrollbar.__styleInstance
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)

            triggerSteppersMode(scrollbar)

            //the following tests are assuming that the steppers are not disabled
            compare(style.isScrollable, true, "Scrollbar is not scrollable, cannot test steppers hover/pressed state.")

            if (style.isVertical) {
                compare(style.flickableItem.atYBeginning, true, "View not scrolled to the beginning as expected.")
                compare(style.flickableItem.atYEnd, false, "View unexpectedly already scrolled to the end.")
            } else {
                compare(style.flickableItem.atXBeginning, true, "View not scrolled to the beginning as expected.")
                compare(style.flickableItem.atXEnd, false, "View unexpectedly already scrolled to the end.")
            }

            //Check that the first stepper is disabled and the second one is in normal state
            checkSteppersColour(scrollbar, "disabled", "normal", "default state before moving mouse")

            //Check that hovering on a disabled stepper does not change its colour
            mouseMove(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "disabled", "normal", "mouse hovered to disabled first stepper")

            //Check that tapping on a disabled stepper does not change its colour
            mousePress(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "disabled", "normal", "mouse pressed on disabled first stepper")
            mouseRelease(firstStepper, firstStepper.width/2, firstStepper.height/2)

            //move mouse away from the steppers
            mouseMove(scrollbar, 0, 0)

            //Scroll to the bottom to check disabled state of the second stepper
            var thumb = getThumb(scrollbar)
            var trough = getTrough(scrollbar)
            if (style.isVertical) {
                mouseMove(thumb, 0, 0)
                mouseDrag(thumb, thumb.width/2, thumb.height/2, 0, trough.height)
                compare(style.flickableItem.atYBeginning, false, "View not scrolled to the bottom as expected.")
                compare(style.flickableItem.atYEnd, true, "View unexpectedly at the beginning.")
            } else {
                mouseMove(thumb, 0, 0)
                mouseDrag(thumb, thumb.width/2, thumb.height/2, trough.width, 0)
                compare(style.flickableItem.atXBeginning, false, "View not scrolled to the bottom as expected.")
                compare(style.flickableItem.atXEnd, true, "View unexpectedly at the beginning.")
            }


            //Check that the first stepper is normal and the second one is in disable state
            checkSteppersColour(scrollbar, "normal", "disabled", "after thumb dragged to bottom and mouse moved away from steppers.")

            //Check that hovering on a disabled stepper does not change its colour
            mouseMove(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "disabled", "thumb at the bottom, mouse hovered on disabled second stepper")

            //Check that tapping on a disabled stepper does not change its colour
            mousePress(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "disabled", "thumb at the bottom, mouse pressed on disabled second stepper")
            mouseRelease(secondStepper, secondStepper.width/2, secondStepper.height/2)

            //move mouse away from the steppers
            mouseMove(scrollbar, 0, 0)

            //trigger first stepper to scroll up a bit, so that they're both not disabled
            //scrolling up with a stepper should not scroll to the top, unless there's a bug somewhere else
            //or the flickable test item gets changed
            clickInTheMiddle(firstStepper)
            checkScrolling(flickable, style.flickableItem.contentX, style.flickableItem.contentY, style,
                           false, -1, "First stepper, click")

            //the previous click ungrabs the mouse after release, so we can't directly send a Move outside
            //the stepper's area because QQuickWindow will just deliver it to the item at that coord, given
            //that there is no item grabbing the mouse. So we have to send a mouse move
            //to the second stepper INSIDE its area, before we can send it an event outside
            //its area in order to trigger HoverLeave.
            mouseMove(secondStepper, 0, 0)

            //send an event outside the area of the second stepper (read comment above). This will
            //trigger HoverLeave for the secondStepper. If you want to know why, see
            //https://github.com/qtproject/qtdeclarative/blob/5.4/src/quick/items/qquickwindow.cpp#L1666
            mouseMove(scrollbar, 0, 0)

            //Check colours in normal state
            checkSteppersColour(scrollbar, "normal", "normal", "mouse moved away from steppers")

            //Hover on first stepper and check colour of both steppers
            mouseMove(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "hovered", "normal", "mouse hovered on first stepper")

            //Press on first stepper and check colour of both steppers
            mousePress(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "pressed", "normal", "mouse pressed on first stepper")
            mouseRelease(firstStepper, firstStepper.width/2, firstStepper.height/2)

            //Hover on second stepper and check colour of both steppers
            mouseMove(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "hovered", "mouse hovered on second stepper")

            //Press on second stepper and check colour of both steppers
            mousePress(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "pressed", "mouse pressed on second stepper")
            mouseRelease(secondStepper, secondStepper.width/2, secondStepper.height/2)

            //Check that pressing on a stepper (mouse-only) will activate the hover state
            //even without a mouse move (i.e. without onEntered/onExited, which can often
            //happen in unit tests)

            //Press on first stepper without first firing mouseMove and check colour of both steppers
            mousePress(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "pressed", "normal", "mouse pressed on first stepper without sending mouseMove before")
            mouseRelease(firstStepper, firstStepper.width/2, firstStepper.height/2)
            checkSteppersColour(scrollbar, "hovered", "normal", "after mouse released while on first stepper")

            //Press on second stepper  without first firing mouseMove and check colour of both steppers
            mousePress(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "pressed", "mouse pressed on second stepper without sending mouseMove before")
            mouseRelease(secondStepper, secondStepper.width/2, secondStepper.height/2)
            checkSteppersColour(scrollbar, "normal", "hovered", "after mouse released while on second stepper")
        }


        //test that moving the mouse inside and outside any of the hover area borders has the expected effect
        function test_thumbHoverArea(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var thumb = getThumb(scrollbar)
            var thumbArea = getThumbArea(scrollbar)
            var trough = getTrough(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var scrollbarUtils = getScrollbarUtils(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var thumbNormalColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 0.4)
            var thumbHoveredColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 0.7)
            var thumbPressedColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 1.0)

            addContentMargins(flickable)

            setContentPositionToTopLeft(flickable)

            triggerSteppersMode(scrollbar)

            //check colour of the thumb in normal state
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true, "Wrong thumb color in normal state.")

            //hover on the middle
            mouseMove(thumb, thumb.width/2, thumb.height/2)
            compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true, "Wrong thumb color in hover state.")

            //check that moving mouse outside the thumb (to the second stepper, in this case) and back in will
            //trigger the correct color changes
            mouseMove(secondStepper, secondStepper.width/2, secondStepper.height/2)
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true, "Wrong thumb color after moving mouse to a stepper.")
            mouseMove(thumb, thumb.width/2, thumb.height/2)
            compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                    "Wrong thumb color after moving mouse from stepper back into the thumb.")

            //check pressed state colour
            mousePress(thumb, thumb.width/2, thumb.height/2)
            compare(Qt.colorEqual(thumb.color, thumbPressedColor), true, "Wrong thumb color in pressed state.")
            mouseRelease(thumb, thumb.width/2, thumb.height/2)

            //check hovered colour again, the thumb should still show as hovered after releasing mouse
            //(assuming the mouse is still inside it, which it is)
            compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                    "Thumb does not show as hovered after mouse press-release inside it.")

            //depending on how the implementation of the scrollbar evolves, the input area
            //may become bigger than the trough on one or both sides, so we map the coords
            //to know where to send events relative to the thumb so that they hit the trough
            //(which is part of the hover area, along the scrolling axis, i.e. x-axis for vert.scrollbar)
            var mappedCoords = thumb.mapToItem(trough, 0, 0)

            //mouseMove seem to deliver events at pixel bound, so we have to take the floor of the sizes
            //otherwise if thumb has width 50.6, mouseMove(thumb, 50.6, 0) will send an event to x==51 and that
            //will cause the logic to assume the mouse is outside the hover area while it's on the border
            var floorThumbWidth = Math.floor(thumb.width)
            var floorThumbHeight = Math.floor(thumb.height)
            var floorTroughWidth = Math.floor(trough.width)
            var floorTroughHeight = Math.floor(trough.height)

            //top-left and bottom-right coords of the rectangle defining the thumb hover area
            //we need different handling of vertical/horizontal because the hover area is defined
            //as the whole trough along the scrolling axis (e.g. whole trough width for the vert. scrollbar)
            //and exactly the thumb along the non-scrolling axis (e.g. thumb's height for vert. scrollbar
            if (style.isVertical) {
                var insideTopLeft     = [-mappedCoords.x                   , 0               ]
                var insideBottomRight = [-mappedCoords.x + floorTroughWidth, floorThumbHeight]
            } else {
                var insideTopLeft     = [0              , -mappedCoords.y                    ]
                var insideBottomRight = [floorThumbWidth, -mappedCoords.y + floorTroughHeight]
            }
            mouseMove(thumb, insideTopLeft[0], insideTopLeft[1])
            compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                    "Thumb does not show as hovered after mouse moved to the top-left corner of the hover area.")
            mouseMove(thumb, insideBottomRight[0], insideBottomRight[1])
            compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                    "Thumb does not show as hovered after mouse moved to the bottom-right corner of the hover area.")

            //right outside the hover area
            var outsideTopBorder    = [insideTopLeft[0]           , insideTopLeft[1] - 1] //move 1 up
            var outsideLeftBorder   = [insideTopLeft[0] - 1       , insideTopLeft[1]] //move 1 left
            var outsideBottomBorder = [insideBottomRight[0]       , insideBottomRight[1] + 1] //move 1 down
            var outsideRightBorder  = [insideBottomRight[0] + 1   , insideBottomRight[1]] //move 1 right

            //check that when the mouse is right outside the borders of the hover area the thumb will use the non-hover colour
            mouseMove(thumb, outsideTopBorder[0], outsideTopBorder[1])
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true,
                    "Thumb does not show as NOT-hovered after mouse moved outside the top border of the hover area.")
            mouseMove(thumb, outsideBottomBorder[0], outsideBottomBorder[1])
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true,
                    "Thumb does not show as NOT-hovered after mouse moved outside the bottom border of the hover area.")
            mouseMove(thumb, outsideLeftBorder[0], outsideLeftBorder[1])
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true,
                    "Thumb does not show as NOT-hovered after mouse moved outside the left border of the hover area.")
            mouseMove(thumb, outsideRightBorder[0], outsideRightBorder[1])
            compare(Qt.colorEqual(thumb.color, thumbNormalColor), true,
                    "Thumb does not show as NOT-hovered after mouse moved outside the right border of the hover area.")

            //BUG #1616926
            //check pressind on the THUMB and releasing mouse outside the TROUGH resets it to normal state
            //press thumb, move mouse outside the trough and check that the thumb is not showing as hovered
            //The broken logic was just checking if mouse was inside the thumb
            //on the same axis as the scrolling one (so y for vertical scrollbar, x for horizontal) so we move
            //mouse in the opposite axis (i.e. along x if vertical scrollbar) to check that the bug is fixed
            mousePress(thumb, thumb.width/2, thumb.height/2)
            if (style.isVertical) {
                mouseMove(thumb, outsideLeftBorder[0], outsideLeftBorder[1])
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true, "Wrong THUMB color in pressed state.")
                mouseRelease(thumb, outsideLeftBorder[0], outsideLeftBorder[1])
                compare(Qt.colorEqual(thumb.color, thumbNormalColor), true, "Wrong THUMB color after releasing mouse with x outside TROUGH.")
            } else {
                mouseMove(thumb, outsideTopBorder[0], outsideTopBorder[1])
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true, "Wrong THUMB color in pressed state.")
                mouseRelease(thumb, outsideTopBorder[0], outsideTopBorder[1])
                compare(Qt.colorEqual(thumb.color, thumbNormalColor), true, "Wrong THUMB color after releasing mouse with y outside TROUGH.")
            }
        }

        //test dragging the thumb and relative visual changes due to hover/pressed states
        function test_dragThumbAndCheckStyling(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var thumb = getThumb(scrollbar)
            var thumbArea = getThumbArea(scrollbar)
            var trough = getTrough(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var scrollbarUtils = getScrollbarUtils(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var thumbNormalColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 0.4)
            var thumbHoveredColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 0.7)
            var thumbPressedColor = Qt.rgba(style.sliderColor.r, style.sliderColor.g, style.sliderColor.b,
                                           style.sliderColor.a * 1.0)

            addContentMargins(flickable)

            setContentPositionToTopLeft(flickable)

            triggerSteppersMode(scrollbar)

            if (style.isVertical) {
                mouseDrag(thumb, thumb.width/2, thumb.height/2, 0, trough.height)
                compare(flickable[scrollbarUtils.propContent],
                        flickable.contentHeight + flickable.bottomMargin - flickable.height,
                        "Vertical thumb mouse drag: wrong contentProp after dragging to the end")

                var sceneThumbY = thumb.mapToItem(column, 0, 0).y

                //Cannot use mouseDrag here, because the thumb is at the end of the trough.
                //mouseDrag uses
                //mouseMove(item, x + util.dragThreshold + 1, y + util.dragThreshold + 1, delay, button)
                //to trigger the drag, but in our case that cannot work because the scrollbar is at
                //the end of the trough, hence cannot move to its
                //bottom (vertical scrollbar) or to its right (in the case of the horiz scrollbar)
                mousePress(thumb, thumb.width/2, thumb.height/2)
                mouseMove(thumb, thumb.width/2, 0  )
                //drag to the beginning
                mouseMove(thumb, thumb.width/2, -sceneThumbY)

                //try moving the mouse outside the thumb (to the left, the 10* is random)
                //and check that it's still using the pressed state colour
                mouseMove(thumb, -10*thumb.width/2, -sceneThumbY)
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true,
                        "Not showing pressed colour after dragging the thumb up and moving mouse outside of it without releasing.")

                //move mouse back inside the thumb
                mouseMove(thumb, thumb.width/2, -sceneThumbY)
                //check pressed colour again
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true,
                        "Not showing pressed colour after dragging the thumb up and moving mouse outside of it and back in, without releasing.")

                mouseRelease(thumb, 0, 0)

                compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                        "Thumb does not show as hovered after mouse is dragged and released inside of it.")

                compare(flickable[scrollbarUtils.propContent], -flickable.topMargin,
                        "Vertical thumb mouse drag: wrong contentProp after dragging to the beginning")
            } else {
                mouseDrag(thumb, thumb.width/2, thumb.height/2, trough.width, 0)
                compare(flickable[scrollbarUtils.propContent],
                        flickable.contentWidth + flickable.rightMargin - flickable.width,
                        "Horizontal thumb mouse drag: wrong contentProp after dragging to the end")

                var sceneThumbX = thumb.mapToItem(column, 0, 0).x
                //Can't use mouseDrag here, see the explanation inside the "if" branch
                mousePress(thumb, thumb.width/2, thumb.height/2)
                mouseMove(thumb, 0, thumb.height/2  )
                //drag to beginning
                mouseMove(thumb, -sceneThumbX, thumb.height/2)

                //try moving the mouse outside the thumb (to the top, the 10* is random)
                //and check that it's still using the pressed state colour
                mouseMove(thumb, -sceneThumbX, -10*thumb.height/2)
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true,
                        "Not showing pressed colour after dragging the thumb up and moving mouse outside of it without releasing.")

                //move mouse back inside the thumb
                mouseMove(thumb, -sceneThumbX, thumb.height/2)

                //check pressed colour again
                compare(Qt.colorEqual(thumb.color, thumbPressedColor), true,
                        "Not showing pressed colour after dragging the thumb up and moving mouse outside of it and back in, without releasing.")

                mouseRelease(thumb, 0, 0)

                compare(Qt.colorEqual(thumb.color, thumbHoveredColor), true,
                        "Thumb does not show as hovered after mouse is dragged and released inside of it.")

                compare(flickable[scrollbarUtils.propContent], -flickable.leftMargin,
                        "Horizontal thumb mouse drag: wrong contentProp after dragging to the beginning")
            }
        }

        function test_hinting_data() {
            return [
                        //ownSide here is the side the scrollable cares about, i.e. height for vert scrollbar
                        {tag: "vertical scrollbar, no scrollable side, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: false, veryLong: false },
                        {tag: "horizontal scrollbar, no scrollable side, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: false, veryLong: false },
                        {tag: "vertical scrollbar, horizontal side scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: false, veryLong: false },
                        {tag: "horizontal scrollbar, horizontal side scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: false, veryLong: false },
                        {tag: "vertical scrollbar, vertical side scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: true, veryLong: false },
                        {tag: "horizontal scrollbar, vertical side scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: true, veryLong: false },
                        {tag: "vertical scrollbar, both sides scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: true, veryLong: false },
                        {tag: "horizontal scrollbar, both sides scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: true, veryLong: false },
                        {tag: "vertical scrollbar, no scrollable side, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: false, veryLong: true },
                        {tag: "horizontal scrollbar, no scrollable side, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: false, veryLong: true },
                        {tag: "vertical scrollbar, horizontal side scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: false, veryLong: true },
                        {tag: "horizontal scrollbar, horizontal side scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: false, veryLong: true },
                        {tag: "vertical scrollbar, vertical side scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: true, veryLong: true },
                        {tag: "horizontal scrollbar, vertical side scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: true, veryLong: true },
                        {tag: "vertical scrollbar, both sides scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: true, veryLong: true },
                        {tag: "horizontal scrollbar, both sides scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: true, veryLong: true },
                    ];
        }

        //test that the scrollbar is shown on contentHeight/width changes
        function test_hinting(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = freshTestItem.scrollbar.__styleInstance

            //we'll add it 1gu to simulate a change that doesn't make it become scrollable
            //so we need to make sure adding 1gu doesn't make it scrollable
            verify(flickable.width > units.gu(5), "Flickable width assumption is satisfied.")
            verify(flickable.height > units.gu(5), "Flickable height assumption is satisfied.")
            flickable.contentWidth = units.gu(2)
            flickable.contentHeight = units.gu(2)
            checkNonScrollableState(scrollbar)

            if (data.scrollableHorizontally) {
                if (data.veryLong) {
                    setVeryLongContentItem(flickable, style, true)
                    tryCompare(style, "veryLongContentItem", true, 5000, "Hinting: veryLongContentItem should be true.")
                } else {
                    flickable.contentWidth = flickable.width + units.gu(1)
                }
                if (!style.isVertical) {
                    checkHinting(scrollbar)
                    //wait for the hinting to finish
                    checkScrollableAndHidden(scrollbar)
                } else {
                    //make sure the scrollbar was not affected by the size change of the opposite side
                    checkNonScrollableState(scrollbar)
                }
            }

            if (data.scrollableVertically) {
                if (data.veryLong) {
                    setVeryLongContentItem(flickable, style, false)
                    tryCompare(style, "veryLongContentItem", true, 5000, "Hinting: veryLongContentItem should be true.")
                } else {
                    flickable.contentHeight = flickable.height + units.gu(1)
                }
                if (style.isVertical) {
                    checkHinting(scrollbar)
                    checkScrollableAndHidden(scrollbar)
                } else {
                    //Only make sure that the horiz scrollbar is not scrollable IF
                    //we didn't previously also make the horizontal side scrollable
                    if (!data.scrollableHorizontally) {
                        checkNonScrollableState(scrollbar)
                    }
                }
            }

            //try changing contentHeight
            flickable.contentHeight += units.gu(1)
            if (style.isVertical && data.scrollableVertically) {
                checkHinting(scrollbar)
                checkScrollableAndHidden(scrollbar)
            } else if (!style.isVertical && data.scrollableHorizontally) {
                compare(style.state, "hidden", "Wrong state.")
            } else {
                checkNonScrollableState(scrollbar)
            }

            flickable.contentWidth += units.gu(1)
            if (!style.isVertical && data.scrollableHorizontally) {
                checkHinting(scrollbar)
                checkScrollableAndHidden(scrollbar)
            } else if (style.isVertical && data.scrollableVertically) {
                compare(style.state, "hidden", "Wrong state.")
            } else {
                checkNonScrollableState(scrollbar)
            }
        }

        function test_minimumSliderSize(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var trough = getTrough(scrollbar)
            var thumb = getThumb(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var minSize = style.minimumSliderSize

            verify(trough !== null, "Could not find the trough.")
            verify(thumb !== null, "Could not find the thumb.")

            if (style.isVertical) {
                //ignore margins etc, just go for an upper bound
                flickable.contentHeight = flickable.height * (flickable.height / minSize) + units.gu(100)
                compare(thumb.height, minSize, "Thumb does not respect the minimum size.")
            } else {
                flickable.contentWidth = flickable.width * (flickable.width / minSize) + units.gu(100)
                compare(thumb.width, minSize, "Thumb does not respect the minimum size.")
            }
        }

        function test_scrollingRatioValuesSanityCheck() {
            var freshTestItem = getFreshFlickable(Qt.AlignTrailing)
            var style = freshTestItem.scrollbar.__styleInstance
            verify(style.shortScrollingRatio > 0, true,
                   "Short scrolling ratio must be > 0.")
            verify(style.longScrollingRatio > 0, true,
                   "Short scrolling ratio must be > 0.")
            verify(style.shortScrollingRatio <= style.longScrollingRatio, true,
                   "Short scrolling ratio is higher than long scrolling ratio.")
        }

        //        NOTE: ANY FAILURE COMING AFTER MOUSEPRESS WILL LEAVE THE MOUSE IN A DIRTY STATE!
        //        (QQuickWindow's mouseGrabberItem is not reset)
        //        OTHER TESTS WILL FAIL JUST BECAUSE THIS TEST HAS NOT CALLED MOUSERELEASE!
        //        I REPORTED https://bugreports.qt.io/browse/QTBUG-51193 TO GET THIS FIXED
        function test_resetDragWhenMouseIsVeryFarFromTheThumb(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var thumb = getThumb(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance

            //SKIP THE TEST IF THE FEATURE IS DISABLED
            verify(style.__disableDragResetFeature !== undefined, "Check that __disableDragResetFeature exists. Remember to enable this test again when the feature is added back.")
            if (style.__disableDragResetFeature) return

            compare(style.isScrollable, true, "Item is assumed to be scrollable.")
            triggerSteppersMode(scrollbar)

            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY

            //coords of the 2 mouse move used to simulate a drag
            var firstStep = units.gu(5)
            var secondStep = units.gu(10)
            var releaseCoord = units.gu(11)

            mousePress(thumb, 0, 0)
            mouseMove(thumb,
                      (style.isVertical ? 0 : firstStep),
                      (style.isVertical ? firstStep : 0))
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep),
                      (style.isVertical ? secondStep : 0))

            var newContentX = flickable.contentX
            var newContentY = flickable.contentY

            if (style.isVertical) {
                verify(newContentY !== contentYBackup, "Check that mouse drag changed contentY." )
            } else {
                verify(newContentX !== contentXBackup, "Check that mouse drag changed contentX." )
            }

            mouseMove(thumb,
                      (style.isVertical ? dragResetThreshold(scrollbar)+1 : secondStep),
                      (style.isVertical ? secondStep : dragResetThreshold(scrollbar)+1))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            //go back to the old position
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep),
                      (style.isVertical ? secondStep : 0))
            //resume the drag by moving at least 1 px (scrollbar should have 0 drag threshold)
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep + style.dragThreshold + 1),
                      (style.isVertical ? secondStep + style.dragThreshold + 1 : 0))

            //NOTE: now the thumb has moved, adding another mouseMove here will use
            //coordinates relative to the NEW position!
            mouseMove(thumb,
                      (style.isVertical ? 0 : -(style.dragThreshold + 1)),
                      (style.isVertical ? -(style.dragThreshold + 1) : 0))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            //test that we resume dragging
            if (style.isVertical) {
                verify(newContentY !== contentYBackup, "Check that mouse drag changed contentY." )
            } else {
                verify(newContentX !== contentXBackup, "Check that mouse drag changed contentX." )
            }

            //try moving the other way
            //FIXME: I'M USING +7 BECAUSE THE LOGIC USES (mouse.x - thumbArea.x)
            //AND HERE WE START AT 5px AND THEN MOVE BY 10x + 1
            //THE LOGIC HAS TO USE A DIFFERENT CONDITION
            mouseMove(thumb,
                      (style.isVertical ? -(dragResetThreshold(scrollbar) + 7) : secondStep),
                      (style.isVertical ? secondStep : -(dragResetThreshold(scrollbar) + 7)))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            mouseRelease(thumb,
                         (style.isVertical ? -(dragResetThreshold(scrollbar) + 7) : secondStep),
                         (style.isVertical ? secondStep : -(dragResetThreshold(scrollbar) + 7)))
        }

        function test_actionSteppers_data() {
            return [
                        {tag:"Content item with margins and vertical scrollbar", addContentMargins: true, alignment: Qt.AlignTrailing},
                        {tag:"Content item and vertical scrollbar", addContentMargins: false, alignment: Qt.AlignTrailing},
                        {tag:"Content item with margins and horizontal scrollbar", addContentMargins: true, alignment: Qt.AlignBottom},
                        {tag:"Content item and horizontal scrollbar", addContentMargins: false, alignment: Qt.AlignBottom}
                    ];
        }
        function test_actionSteppers(data) {
            skip("Scrollbar already in steppers mode under Qt 5.9, causing failure.")
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var trough = getTrough(scrollbar)
            var thumb = getThumb(scrollbar)
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var minSize = style.minimumSliderSize

            if (data.addContentMargins) {
                addContentMargins(flickable)
            }

            triggerSteppersMode(scrollbar)

            setContentPositionToTopLeft(flickable)
            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY
            clickOnStepperAndCheckNoContentPositionChange("First stepper", firstStepper, flickable,
                                                          contentXBackup, contentYBackup)

            //NOTE: mouseClick(secondStepper, 0, 0) would trigger firstStepper, because
            //the logic uses contains(point) that also counts the edges!
            //(the code uses if firstStepper.contains else if secondStepper.contains
            //so it stops at the first branch)
            clickInTheMiddle(secondStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, 1, "Second stepper, first click")

            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            //do it once more
            clickInTheMiddle(secondStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, 1, "Second stepper, second click")

            setContentPositionToBottomRight(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            clickOnStepperAndCheckNoContentPositionChange("Second stepper", secondStepper, flickable,
                                                          contentXBackup, contentYBackup)

            clickInTheMiddle(firstStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, -1, "First stepper, first click")

            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            //do it once more
            clickInTheMiddle(firstStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, -1, "First stepper, second click")

        }

        //check that the mouse filters handling the interactions with the thumb and steppers
        //ignore the synthesized events. This is required to have the correct hover handling,
        //because it's the way we make sure that the hover logic is only called for real mouse
        //events, and not touch-to-mouse synthesized events
        function test_ignoreSynthesizedEventsInMouseFilters() {
            var scrollbar = defaultValuesScrollbar
            var style = scrollbar.__styleInstance
            var thumbArea = getThumbArea(scrollbar)
            var steppersMouseArea = getSteppersMouseArea(scrollbar)

            compare(thumbArea.Mouse.ignoreSynthesizedEvents, true,
                    "The mouse filter in the thumb MouseArea does not ignore synthesized events. Hover state logic may be affected.")
            compare(steppersMouseArea.Mouse.ignoreSynthesizedEvents, true,
                    "The mouse filter in the steppers MouseArea does not ignore synthesized events. Hover state logic may be affected.")
        }

        function test_defaultStylingValues() {
            var scrollbar = defaultValuesScrollbar
            var style = scrollbar.__styleInstance

            /*****************************************************
             *      STYLING PROPERTIES                           *
             *****************************************************/
            compare(style.interactive, style.isMouseConnected || style.veryLongContentItem, "Wrong styling property default value.")
            compare(style.minimumSliderSize, units.gu(1), "Wrong styling property default value.")
            compare(style.overlay, !style.alwaysOnScrollbars, "Wrong styling property default value.")
            compare(style.overlayOpacityWhenShown, 1.0, "Wrong styling property default value.")
            compare(style.overlayOpacityWhenHidden, 0.0, "Wrong styling property default value.")
            compare(style.troughThicknessSteppersStyle, units.dp(14), "Wrong styling property default value.")
            compare(style.troughThicknessThumbStyle, units.dp(14), "Wrong styling property default value.")
            compare(style.troughThicknessIndicatorStyle, units.dp(9), "Wrong styling property default value.")
            compare(style.troughColorThumbStyle, theme.palette.normal.foreground, "Wrong styling property default value.")
            compare(style.troughColorSteppersStyle, theme.palette.normal.foreground, "Wrong styling property default value.")
            compare(style.stepperBgColor, theme.palette.normal.base, "Wrong styling property default value.")
            compare(style.sliderColor, theme.palette.normal.overlayText, "Wrong styling property default value.")
            compare(style.sliderRadius, units.dp(3), "Wrong styling property default value.")
            compare(style.thumbThickness, units.gu(1), "Wrong styling property default value.")
            compare(style.indicatorThickness, units.dp(3), "Wrong styling property default value.")
            compare(style.scrollbarThicknessAnimation.duration, UbuntuAnimation.SnapDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeInAnimation.duration, UbuntuAnimation.SlowDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeOutAnimation.duration, UbuntuAnimation.SlowDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeOutPause, 3000, "Wrong styling property default value.")
            compare(style.scrollbarCollapsePause, 1000, "Wrong styling property default value.")
            compare(style.shortScrollingRatio, 0.1, "Wrong styling property default value.")
            compare(style.longScrollingRatio, 0.9, "Wrong styling property default value.")
            compare(style.hintingStyle, style.veryLongContentItem ? 'thumb' : 'indicator', "Wrong styling property default value.")
            compare(style.thumbsExtremesMargin, units.dp(4), "Wrong styling property default value.")

            /*****************************************************
             *      HELPER PROPERTIES                            *
             *****************************************************/
            var slider = getThumb(scrollbar)
            var trough = getTrough(scrollbar)
            compare(style.thumb, slider, "Wrong styling property default value.")
            compare(style.trough, trough, "Wrong styling property default value.")
            compare(style.isScrollable, scrollbar.__private.scrollable && style.pageSize > 0.0
                    && style.contentSize > 0.0 && style.totalContentSize > style.pageSize, "Wrong styling property default value.")
            compare(style.isVertical, (scrollbar.align === Qt.AlignLeading) || (scrollbar.align === Qt.AlignTrailing), "Wrong styling property default value.")
            compare(style.frontAligned, scrollbar.align === Qt.AlignLeading, "Wrong styling property default value.")
            compare(style.rearAligned, scrollbar.align === Qt.AlignTrailing, "Wrong styling property default value.")
            compare(style.topAligned, scrollbar.align === Qt.AlignTop, "Wrong styling property default value.")
            compare(style.bottomAligned, scrollbar.align === Qt.AlignBottom, "Wrong styling property default value.")

            //flickable helper properties
            compare(style.flickableItem, scrollbar.flickableItem, "Wrong styling property default value.")
            compare(style.pageSize, (style.isVertical) ? (scrollbar.flickableItem.height) : (scrollbar.flickableItem.width), "Wrong styling property default value.")
            compare(style.contentSize, (style.isVertical) ? scrollbar.flickableItem.contentHeight : scrollbar.flickableItem.contentWidth, "Wrong styling property default value.")
            compare(style.leadingContentMargin, style.isVertical ? scrollbar.flickableItem.topMargin : scrollbar.flickableItem.leftMargin, "Wrong styling property default value.")
            compare(style.trailingContentMargin, style.isVertical ? scrollbar.flickableItem.bottomMargin : scrollbar.flickableItem.rightMargin, "Wrong styling property default value.")
            compare(style.totalContentSize, style.contentSize + style.leadingContentMargin + style.trailingContentMargin, "Wrong styling property default value.")

            /*****************************************************
             *      INTERNAL PROPERTIES AND FUNCTIONS            *
             *****************************************************/
            var thumbArea = getThumbArea(scrollbar)
            var scrollAnimation = getScrollAnimation(scrollbar)
            compare(style.__recursionGuard, false, "Wrong styling property default value.")
            compare(style.__disableStateBinding, false, "Wrong styling property default value.")
            compare(style.__hinting, false, "Wrong styling property default value.")
            compare(style.draggingThumb, thumbArea.drag.active || slider.mouseDragging || slider.touchDragging, "Wrong styling property default value.")
            compare(style.thumbStyleFlag, style.veryLongContentItem && (scrollbar.flickableItem.moving || scrollAnimation.running), "Wrong styling property default value.")
            compare(style.veryLongContentItem, scrollbar.flickableItem && style.initialized
                    && ((scrollbar.flickableItem.contentHeight > scrollbar.flickableItem.height * 10)
                        || (scrollbar.flickableItem.contentWidth > scrollbar.flickableItem.width * 10)), "Wrong styling property default value.")
            compare(style.isMouseConnected, true, "Wrong styling property default value.")
            compare(style.nonOverlayScrollbarMargin, style.troughThicknessSteppersStyle, "Wrong styling property default value.")
            compare(style.useSteppers, true, "Wrong styling property default value.")
            compare(style.touchDragStartMargin, units.gu(2), "Wrong styling property default value.")
            compare(style.dragThreshold, units.dp(2), "Wrong styling property default value.")
            compare(style.alwaysOnScrollbars, scrollbar.__alwaysOnScrollbars, "Wrong styling property default value.")
        }

        //just a few tests to ensure the key variables don't get out of range values
        //NOTE: this does not test that the properties have the correct value, just that
        //their values is not out of range (so it only applies to numeric values)
        function test_sanityCheckStylingVariables() {
            flickableSanityCheck.contentHeight = contentSanityCheck.height
            flickableSanityCheck.contentWidth = contentSanityCheck.width
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.contentHeight = -contentSanityCheck.height
            flickableSanityCheck.contentWidth = -contentSanityCheck.width
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.topMargin = units.gu(1)
            flickableSanityCheck.leftMargin = units.gu(2)
            flickableSanityCheck.rightMargin = units.gu(3)
            flickableSanityCheck.bottomMargin = units.gu(4)
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.topMargin = -units.gu(1)
            flickableSanityCheck.leftMargin = -units.gu(2)
            flickableSanityCheck.rightMargin = -units.gu(3)
            flickableSanityCheck.bottomMargin = -units.gu(4)
            performStyleSanityCheck(scrollbarSanityCheck)
        }

        //Test that Scrollbar defaults to Flickable.topMargin/bottomMargin (content margins)
        //as its ANCHORS margins
        function test_topBottomMargins(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            compare(scrollbar.__styleInstance.isScrollable, true, "The view is not scrollable.")
            compare(scrollbar.__styleInstance.alwaysOnScrollbars, false, "This test assumes overlay scrollbars (if you need to change this, fix the expected margins).")
            compare(scrollbar.anchors.topMargin, flickable.topMargin, "Wrong anchors.topMargin.")
            compare(scrollbar.anchors.bottomMargin, flickable.bottomMargin, "Wrong anchors.bottomMargin.")

            flickable.topMargin = units.gu(1)
            compare(scrollbar.anchors.topMargin, units.gu(1), "Wrong anchors.topMargin.")
            compare(scrollbar.anchors.bottomMargin, flickable.bottomMargin, "Wrong anchors.bottomMargin.")

            flickable.bottomMargin = units.gu(3)
            compare(scrollbar.anchors.topMargin, units.gu(1), "Wrong anchors.topMargin.")
            compare(scrollbar.anchors.bottomMargin, units.gu(3), "Wrong anchors.bottomMargin.")

            flickable.topMargin = 0
            flickable.bottomMargin = units.gu(6)
            compare(scrollbar.anchors.topMargin, 0, "Wrong anchors.topMargin.")
            compare(scrollbar.anchors.bottomMargin, units.gu(6), "Wrong anchors.bottomMargin.")
        }

        //check that we don't output lots of warnings when the flickable item becomes null
        //(it could happen while transitioning values or before the Binding applies and similar situations)
        function test_noWarningsWhenFlickableIsNull() {
            var freshTestItem = getFreshFlickable(Qt.AlignTrailing)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            verify(scrollbar.flickableItem !== null, "Check that Scrollbar is linked to a Flickable.")
            //try triggering warnings (they will be marked as failures by the SDK test runner)
            scrollbar.flickableItem = null

            var secondScrollbar = freshScrollbar.createObject(freshTestItem)
            verify(secondScrollbar !== null, "Error: dynamic item creation failed.")

            var style = secondScrollbar.__styleInstance

            //check that the default value is correct
            compare(style.flickableItem, null, "Wrong style flickableItem")
            compare(scrollbar.flickableItem, null, "Wrong scrollbar's flickableItem")
            compare(scrollbar.__initializedFlickable, null, "Wrong scrollbar's initialized flickable var")

            //check that calling functions that rely on flickableItem doesn't output errors
            style.scrollToBeginning()
            style.scrollToEnd()

            //This test will always pass if run with qmltestrunner, unfortunately there's no way
            //using TestCase to do "if (testOutputsWarnings) --> fail", but the SDK test script
            //will fail if this test outputs warnings, which is what we want
        }

        //test that __initializedFlickable has the right value even when
        //the Scrollbar is assigned to one Flickable, then to another one (this could
        //be useful to reuse the same instance of Scrollbar for multiple Flickables
        //if they're only visible one at a time)
        function test_initializedFlickableVariableChange() {
            var freshTestItem = getFreshFlickable(Qt.AlignTrailing)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            //check that the variables are correct when the Scrollbar is initialized with a set Flickable
            compare(scrollbar.flickableItem, flickable, "Wrong value of flickableItem")
            compare(scrollbar.__initializedFlickable, flickable, "Wrong value of __initializedFlickable")

            var secondScrollbar = freshScrollbar.createObject(freshTestItem)
            verify(secondScrollbar !== null, "Error: dynamic item creation failed.")

            //check that the variables are correct when the Scrollbar is assigned a new Flickable
            secondScrollbar.flickableItem = flickable
            compare(secondScrollbar.flickableItem, flickable, "Wrong value of flickableItem")
            compare(secondScrollbar.__initializedFlickable, flickable, "Wrong value of __initializedFlickable")

            //set to null and check that both change
            secondScrollbar.flickableItem = null
            compare(secondScrollbar.flickableItem, null, "Wrong value of flickableItem")
            compare(secondScrollbar.__initializedFlickable, null, "Wrong value of __initializedFlickable")

            //set to a valid flickable again and check that both the variables are correctly updated
            secondScrollbar.flickableItem = flickable
            compare(secondScrollbar.flickableItem, flickable, "Wrong value of flickableItem")
            compare(secondScrollbar.__initializedFlickable, flickable, "Wrong value of __initializedFlickable")

            secondScrollbar.destroy()
        }
    }
}
