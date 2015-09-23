/*
 * Copyright 2015 Canonical Ltd.
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

    Flickable {
        anchors.fill: parent
        Column {
            id: column
            Rectangle {
                //Y here should be 21.5 but it gets rounded to 21...bug in QtQuick's anchors?
                id: verticalCenterBuggyTestCase
                height: 54.5
                Item {
                    height: 11.5
                    anchors.verticalCenter: parent.verticalCenter
                    onYChanged: console.log("Buggy Anchors? Y should be 21.5, but it is", y)
                }
            }

            Repeater {
                model: layoutsModel
            }
        }
    }

    VisualItemModel {
        id: layoutsModel        
        SlotsLayout {
            id: layout
            //these lists define the slots that we expect to be part of the layout
            //process, and the order in which we expect them to be positioned in the
            //layout. This is to avoid complicating the layout logic by adding conditions
            //to the method which tests the position of the slots. If we want to
            //test that a !visible slot is ignored during the relayout, for instance,
            //we have to remove it from these lists before we call checkSlotsPosition(layout).
            //The same applies to changes in the position of a slot.
            readonly property var leadingSlots: []
            readonly property var trailingSlots: []
        }
        ListItemLayout {
            id: layoutEmpty
            readonly property var leadingSlots: []
            readonly property var trailingSlots: []
        }
        ListItemLayout {
            id: layoutLabels
            title.text: "test"
            subtitle.text: "test2"
            readonly property var leadingSlots: []
            readonly property var trailingSlots: []
        }
        ListItemLayout {
            id: layoutOneLeading
            readonly property var leadingSlots: [layoutOneLeading_leading1]
            readonly property var trailingSlots: []
            Item { id: layoutOneLeading_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(3); height: units.gu(2) }
        }
        ListItemLayout {
            id: layoutOneTrailing
            readonly property var leadingSlots: []
            readonly property var trailingSlots: [layoutOneTrailing_trailing1]
            Item { id: layoutOneTrailing_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(2) }
        }
        ListItemLayout {
            id: layoutOneLeadingOneTrailing
            readonly property var leadingSlots: [layoutOneLeadingOneTrailing_leading1]
            readonly property var trailingSlots: [layoutOneLeadingOneTrailing_trailing1]
            Item { id: layoutOneLeadingOneTrailing_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(10); height: units.gu(20) }
            Item { id: layoutOneLeadingOneTrailing_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(1); height: units.gu(5) }
        }
        ListItemLayout {
            id: layoutTwoTrailing
            readonly property var leadingSlots: []
            readonly property var trailingSlots: [layoutTwoTrailing_trailing1, layoutTwoTrailing_trailing2]
            Item { id: layoutTwoTrailing_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(3) }
            Item { id: layoutTwoTrailing_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(2) }
        }
        ListItemLayout {
            id: layoutOneLeadingTwoTrailing
            readonly property var leadingSlots: [layoutOneLeadingTwoTrailing_leading1]
            readonly property var trailingSlots: [layoutOneLeadingTwoTrailing_trailing1, layoutOneLeadingTwoTrailing_trailing2]
            Item { id: layoutOneLeadingTwoTrailing_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(3); height: units.gu(1) }
            Item { id: layoutOneLeadingTwoTrailing_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(4); height: units.gu(9) }
            Item { id: layoutOneLeadingTwoTrailing_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(1); height: units.gu(6) }
        }
        ListItemLayout {
            id: layoutCustomSlotsPadding
            readonly property var leadingSlots: [layoutCustomSlotsPadding_leading1]
            readonly property var trailingSlots: [layoutCustomSlotsPadding_trailing1, layoutCustomSlotsPadding_trailing2]
            Item {
                id: layoutCustomSlotsPadding_leading1;
                SlotsLayout.position: SlotsLayout.Leading;
                SlotsLayout.padding.top: units.gu(30)
                SlotsLayout.padding.bottom: units.gu(2)
                width: units.gu(6);
                height: units.gu(3)
            }
            Item {
                id: layoutCustomSlotsPadding_trailing1;
                SlotsLayout.position: SlotsLayout.Trailing;
                SlotsLayout.padding.top: units.gu(30)
                SlotsLayout.padding.bottom: units.gu(2)
                width: units.gu(1);
                height: units.gu(7)
            }
            Item {
                id: layoutCustomSlotsPadding_trailing2;
                SlotsLayout.position: SlotsLayout.Trailing;
                SlotsLayout.padding {
                    leading: units.gu(4)
                    trailing: units.gu(5)
                    top: units.gu(5)
                    bottom: units.gu(2)
                }
                width: units.gu(3);
                height: units.gu(9)
            }
        }
        ListItemLayout {
            id: layoutCustomPadding
            readonly property var leadingSlots: [layoutCustomPadding_leading1]
            readonly property var trailingSlots: [layoutCustomPadding_trailing1, layoutCustomPadding_trailing2]
            padding {
                top: units.gu(1)
                bottom: units.gu(2)
                leading: units.gu(3)
                trailing: units.gu(4)
            }
            Item { id: layoutCustomPadding_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(6); height: units.gu(3) }
            Item { id: layoutCustomPadding_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(1); height: units.gu(7) }
            Item { id: layoutCustomPadding_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(9) }
        }
        ListItemLayout {
            id: layoutTestChangeSlotsSize
            readonly property var leadingSlots: [layoutTestChangeSlotsSize_leading1]
            readonly property var trailingSlots: []
            title.text: "Test"
            padding {
                top: units.gu(0)
                bottom: units.gu(2)
            }
            Item { id: layoutTestChangeSlotsSize_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(10); height: 38.5/*units.gu(3.1222)*/ }
        }
        ListItemLayout {
            id: layoutTestMainSlotSize
            width: units.gu(40)
            readonly property var leadingSlots: [layoutTestMainSlotSize_leading1]
            readonly property var trailingSlots: []
            title.text: "Test"
            Item { id: layoutTestMainSlotSize_leading1; SlotsLayout.position: SlotsLayout.Leading; width: units.gu(1); height: units.gu(3) }
        }
        ListItemLayout {
            id: layoutTestSmallerTopBottomPadding
            readonly property var leadingSlots: [layoutTestSmallerTopBottomPadding_leading1]
            readonly property var trailingSlots: []
            title.text: "Test"
            subtitle.text: "test2"
            summary.text: "test3"
            Item { id: layoutTestSmallerTopBottomPadding_leading1; SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(1); height: Math.max(parent.mainSlot.height, units.gu(4)) }
        }
        ListItemLayout {
            id: layoutTestChangeSlotPosition
            property var leadingSlots: [layoutTestChangeSlotPosition_leading]
            property var trailingSlots: []
            title.text: "Test"
            subtitle.text: "test2"
            summary.text: "test3"
            Item { id: layoutTestChangeSlotPosition_leading; SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(4); height: units.gu(4) }
        }
        ListItemLayout {
            id: layoutTestSlotVisibilityChange
            property var leadingSlots: [layoutTestSlotVisibilityChange_leading]
            readonly property var trailingSlots: []
            title.text: "Test"
            subtitle.text: "test2"
            summary.text: "test3"
            //it has to be taller than mainSlot, so that when we make the slot invisible we can test
            //implicitHeight changes
            Item { id: layoutTestSlotVisibilityChange_leading; SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(4); height: parent.mainSlot.height+10 }
        }
        ListItemLayout {
            //let's test if the layout respects SlotsLayout.position even if that doesn't follow
            //the stacking order
            id: layoutTestRelativePositioning
            //these lists hold the slots in the order we want them to be positioned
            readonly property var leadingSlots: [layoutTestRelativePositioning_leading1]
            readonly property var trailingSlots: [layoutTestRelativePositioning_trailing2, layoutTestRelativePositioning_trailing1]
            Item { id: layoutTestRelativePositioning_leading1; SlotsLayout.position: SlotsLayout.Leading-3; width: units.gu(6); height: units.gu(3) }
            Item { id: layoutTestRelativePositioning_trailing1; SlotsLayout.position: SlotsLayout.Last; width: units.gu(1); height: units.gu(7) }
            Item { id: layoutTestRelativePositioning_trailing2; SlotsLayout.position: SlotsLayout.Last-2; width: units.gu(3); height: units.gu(9) }
        }
        ListItemLayout {
            id: layoutTestRelativePositioning2
            //these lists hold the slots in the order we want them to be positioned
            readonly property var leadingSlots: [layoutTestRelativePositioning2_leading1]
            readonly property var trailingSlots: [layoutTestRelativePositioning2_trailing2, layoutTestRelativePositioning2_trailing1]
            Item { id: layoutTestRelativePositioning2_leading1; SlotsLayout.position: SlotsLayout.Leading-3; width: units.gu(6); height: units.gu(3) }
            Item { id: layoutTestRelativePositioning2_trailing1; SlotsLayout.position: SlotsLayout.Trailing+1; width: units.gu(1); height: units.gu(7) }
            Item { id: layoutTestRelativePositioning2_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(9) }
        }
    }

    TestCase {
        name: "SlotsLayout"
        when: windowShown

        //Visual rule:
        //when we have at least one slot which is taller than mainSlot and taller than 4GU,
        //we want the top and bottom padding to be 1GU instead of 2GU
        property real topBottomPaddingThreshold: units.gu(4)
        property real smallerTopBottomPadding: units.gu(1)
        property real tallerTopBottomPadding: units.gu(2)

        function standardTestsData() {
            return [
                        { tag: "Empty", item: layout  },
                        { tag: "Labels", item: layoutLabels },
                        { tag: "One leading slot", item: layoutOneLeading },
                        { tag: "One trailing slot", item: layoutOneTrailing },
                        { tag: "One leading, one trailing slots", item: layoutOneLeadingOneTrailing },
                        { tag: "Two trailing", item: layoutTwoTrailing },
                        { tag: "One leading, two trailing slots", item: layoutOneLeadingTwoTrailing },
                        { tag: "One leading, two trailing slots with custom paddings", item: layoutCustomSlotsPadding },
                    ]
        }

        //This excludes mainSlot and the slots which have overrideVerticalPositioning enabled!
        function maxSlotsHeight(item) {
            var maxHeight = 0
            var i = 0
            for (i = 0; i < item.leadingSlots.length; ++i) {
                var slot = item.leadingSlots[i]
                if (!slot.SlotsLayout.overrideVerticalPositioning)  {
                    maxHeight = Math.max(slot.height + slot.SlotsLayout.padding.top
                                         + slot.SlotsLayout.padding.bottom, maxHeight)
                }
            }
            for (i = 0; i < item.trailingSlots.length; ++i) {
                var currSlot = item.trailingSlots[i]
                if (!currSlot.SlotsLayout.overrideVerticalPositioning)  {
                    maxHeight = Math.max(currSlot.height + currSlot.SlotsLayout.padding.top
                                         + currSlot.SlotsLayout.padding.bottom, maxHeight)
                }
            }
            return maxHeight
        }

        function expectedImplicitHeight(item) {
            var height = maxSlotsHeight(item)
            if (item.mainSlot !== null) {
                height = Math.max(height, item.mainSlot.height)
            }
            return height + item.padding.top + item.padding.bottom
        }

        //otherwise center the slots vertically
        function mustAlignSlotsToTop(item) {
            if (item.mainSlot === null) {
                return false
            }
            return maxSlotsHeight(item) < item.mainSlot.height
        }
        function useSmallerTopBottomPadding(item) {
            return !mustAlignSlotsToTop(item)
                    && maxSlotsHeight(item) > topBottomPaddingThreshold
        }

        function checkDefaultPadding(item) {
            if (useSmallerTopBottomPadding(item)) {
                compare(item.padding.top, smallerTopBottomPadding, "Default smaller top padding")
                compare(item.padding.bottom, smallerTopBottomPadding, "Default smaller bottom padding")
            } else {
                compare(item.padding.top, tallerTopBottomPadding, "Default top padding")
                compare(item.padding.bottom, tallerTopBottomPadding, "Default bottom padding")
            }
            compare(item.padding.leading, units.gu(1), "Default leading padding")
            compare(item.padding.trailing, units.gu(1), "Default trailing padding")
        }

        function checkImplicitSize(item) {
            compare(item.implicitHeight, expectedImplicitHeight(item), "Implicit height check")
            compare(item.implicitWidth, column.width, "Fill parent's width")
        }

        //this functions takes a layouts and checks that the slots in the lists
        //"leadingSlots" and "trailingSlots" are following the visual rules
        //NOTE: THIS METHOD DOESN'T IGNORE ANY SLOT (because of visibility or similar).
        //slots which are expected to be ignored by the cpp implementation should be
        //removed from "leadingSlots" and "trailingSlots" before calling this method
        function checkSlotsPosition(item) {
            var slots = []
            slots = slots.concat(item.leadingSlots)
            if (item.mainSlot !== null) {
                slots = slots.concat(item.mainSlot)
            }
            slots = slots.concat(item.trailingSlots)

            var expectedX = item.padding.leading;
            var i = 0
            for (i = 0; i < slots.length; ++i) {
                var slot = slots[i]

                expectedX += slot.SlotsLayout.padding.leading
                compare(slot.x, expectedX, "Slot's horizontal position")
                expectedX += slot.width
                expectedX += slot.SlotsLayout.padding.trailing

                if (slot.SlotsLayout.overrideVerticalPositioning) {
                    compare(slot.y, 0, "Override vertical positioning: vertical position")
                } else {
                    if (mustAlignSlotsToTop(item)) {
                        compare(slot.y, item.padding.top + slot.SlotsLayout.padding.top, "Automatic vertical positioning: vertical position, aligned to the top")
                    } else {
                        compare(slot.y, (item.height - slot.height
                                         + item.padding.top - item.padding.bottom
                                         + slot.SlotsLayout.padding.top - slot.SlotsLayout.padding.bottom) / 2.0 ,
                                "Automatic vertical positioning: vertical position, vertically centered" )
                    }
                }
            }
        }

        function test_contentPadding_data() {
            return standardTestsData()
        }
        function test_contentPadding(data) {
            checkDefaultPadding(data.item)
        }

        function test_implicitSize_data() {
            return standardTestsData()
        }
        function test_implicitSize(data) {
            checkImplicitSize(data.item)
        }

        function test_layout_data(){
            return standardTestsData()
        }
        function test_layout(data) {
            checkSlotsPosition(data.item)
        }

        function test_customPadding_data(){
            return [
                        { tag: "Custom padding", item: layoutCustomPadding },
                    ]
        }
        function test_customPadding(data) {
            compare(data.item.padding.top, units.gu(1), "Custom padding top")
            compare(data.item.padding.bottom, units.gu(2), "Custom padding bottom")
            compare(data.item.padding.leading, units.gu(3), "Custom padding leading")
            compare(data.item.padding.trailing, units.gu(4), "Custom padding trailing")
            compare(data.item.implicitHeight, expectedImplicitHeight(data.item),
                    "Implicit height with custom padding")

            //trigger relayout
            var newLeadingSlotHeight = data.item.height + units.gu(2)
            layoutCustomPadding.leadingSlots[0].height = newLeadingSlotHeight;
            compare(data.item.leadingSlots[0].height, newLeadingSlotHeight, "Leading slot height update")

            //check that the padding stays the same, but the height changes
            compare(data.item.padding.top, units.gu(1), "Custom padding top")
            compare(data.item.padding.bottom, units.gu(2), "Custom padding bottom")
            compare(data.item.padding.leading, units.gu(3), "Custom padding leading")
            compare(data.item.padding.trailing, units.gu(4), "Custom padding trailing")
            compare(data.item.implicitHeight, newLeadingSlotHeight + data.item.padding.top + data.item.padding.bottom,
                    "Implicit height with custom padding after relayout")
            compare(expectedImplicitHeight(data.item), newLeadingSlotHeight + data.item.padding.top + data.item.padding.bottom,
                    "expectedImplicitHeight test")

            //check that slots are still in the right position
            checkSlotsPosition(data.item)
        }

        /* FIXME: currently disabled while I investigate why setting anchors.verticalCenter some times
                  sets the wrong Y (usually +/- 0.5 from the correct value)
        function test_relayoutAfterChangingSlotsSize() {
            checkSlotsPosition(layoutTestChangeSlotsSize)

            layoutTestChangeSlotsSize.leadingSlots[0].width = units.gu(12)
            compare(layoutTestChangeSlotsSize.leadingSlots[0].width, units.gu(12), "Change slot's width")
            checkSlotsPosition(layoutTestChangeSlotsSize)

            layoutTestChangeSlotsSize.leadingSlots[0].height = layoutTestChangeSlotsSize.mainSlot.height
            compare(layoutTestChangeSlotsSize.leadingSlots[0].height,
                    layoutTestChangeSlotsSize.mainSlot.height, "Change slot's height")
            compare(layoutTestChangeSlotsSize.implicitHeight,
                    layoutTestChangeSlotsSize.mainSlot.height
                    + layoutTestChangeSlotsSize.padding.top
                    + layoutTestChangeSlotsSize.padding.bottom,
                    "Layout's implicit height check after slot's height change")
            checkSlotsPosition(layoutTestChangeSlotsSize)
        }*/

        function test_mainSlotSize() {
            compare(layoutTestMainSlotSize.mainSlot.width,
                    layoutTestMainSlotSize.width
                    - layoutTestMainSlotSize.padding.leading - layoutTestMainSlotSize.padding.trailing
                    - layoutTestMainSlotSize.leadingSlots[0].width
                    - layoutTestMainSlotSize.leadingSlots[0].SlotsLayout.padding.leading
                    - layoutTestMainSlotSize.leadingSlots[0].SlotsLayout.padding.trailing
                    - layoutTestMainSlotSize.mainSlot.SlotsLayout.padding.leading
                    - layoutTestMainSlotSize.mainSlot.SlotsLayout.padding.trailing,
                    "Main slot's width")

            compare(layoutTestMainSlotSize.mainSlot.height,
                    layoutTestMainSlotSize.title.height,
                    "Main slot's height, title only")

            layoutTestMainSlotSize.subtitle.text = "Test here"
            compare(layoutTestMainSlotSize.mainSlot.height,
                    layoutTestMainSlotSize.subtitle.y + layoutTestMainSlotSize.subtitle.height,
                    "Main slot's height, title+subtitle")

            layoutTestMainSlotSize.summary.text = "Test here"
            compare(layoutTestMainSlotSize.mainSlot.height,
                    layoutTestMainSlotSize.summary.y + layoutTestMainSlotSize.summary.height,
                    "Main slot's height, title+subtitle+summary")

            checkSlotsPosition(layoutTestMainSlotSize)
        }

        function test_changeSlotPosition() {
            checkSlotsPosition(layoutTestChangeSlotPosition)
            layoutTestChangeSlotPosition.leadingSlots[0].SlotsLayout.position = SlotsLayout.Trailing
            compare(layoutTestChangeSlotPosition.leadingSlots[0].SlotsLayout.position, SlotsLayout.Trailing, "Slot's position change to Trailing")

            layoutTestChangeSlotPosition.trailingSlots = [layoutTestChangeSlotPosition.leadingSlots[0]]
            layoutTestChangeSlotPosition.leadingSlots = []
            compare(layoutTestChangeSlotPosition.leadingSlots.length, 0, "Change slot's position, leading slots array")
            compare(layoutTestChangeSlotPosition.trailingSlots.length, 1, "Change slot's position, trailing slots array")
            checkSlotsPosition(layoutTestChangeSlotPosition)

            layoutTestChangeSlotPosition.trailingSlots[0].SlotsLayout.position = SlotsLayout.First
            compare(layoutTestChangeSlotPosition.trailingSlots[0].SlotsLayout.position, SlotsLayout.First, "Slot's position change to First")
            layoutTestChangeSlotPosition.leadingSlots = [layoutTestChangeSlotPosition.trailingSlots[0]]
            layoutTestChangeSlotPosition.trailingSlots = []
            compare(layoutTestChangeSlotPosition.leadingSlots.length, 1, "Change slot's position, leading slots array")
            compare(layoutTestChangeSlotPosition.trailingSlots.length, 0, "Change slot's position, trailing slots array")
            checkSlotsPosition(layoutTestChangeSlotPosition)
        }

        function test_slotVisibilityChange() {
            layoutTestSlotVisibilityChange.leadingSlots[0].visible = false
            compare(layoutTestSlotVisibilityChange.leadingSlots[0].visible, false, "Slot's visibility, false")
            var tmpSlot = layoutTestSlotVisibilityChange.leadingSlots[0]
            layoutTestSlotVisibilityChange.leadingSlots = []
            compare(layoutTestSlotVisibilityChange.leadingSlots.length, 0, "Slot's visibility, fake leading slots count")
            compare(layoutTestSlotVisibilityChange.trailingSlots.length, 0, "Slot's visibility, fake trailing slots count")

            checkSlotsPosition(layoutTestSlotVisibilityChange)
            checkImplicitSize(layoutTestSlotVisibilityChange)

            layoutTestSlotVisibilityChange.leadingSlots = [tmpSlot]
            layoutTestSlotVisibilityChange.leadingSlots[0].visible = true
            compare(layoutTestSlotVisibilityChange.leadingSlots[0].visible, true, "Slot's visibility, true")
            compare(layoutTestSlotVisibilityChange.mainSlot.height
                    < layoutTestSlotVisibilityChange.leadingSlots[0].height, true, "Visibility test, slot's height")
            checkSlotsPosition(layoutTestSlotVisibilityChange)
            checkImplicitSize(layoutTestSlotVisibilityChange)
        }


        function test_relativePositioning_data(){
            return [
                        { tag: "Relative positioning 1", item: layoutTestRelativePositioning },
                        { tag: "Relative positioning 2", item: layoutTestRelativePositioning2 }
                    ]
        }
        function test_relativePositioning(data) {
            checkSlotsPosition(data.item)
        }
    }
}
