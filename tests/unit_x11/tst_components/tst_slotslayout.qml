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
            summary.text: "test3"
            readonly property var leadingSlots: []
            readonly property var trailingSlots: []
        }
        ListItemLayout {
            id: layoutMultilineLabels
            title.text: "test \n test"
            title.maximumLineCount: 2
            subtitle.text: "test2 \n test2 \n test2"
            subtitle.maximumLineCount: 3
            summary.text: "test3 \n test3"
            summary.maximumLineCount: 2
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
            readonly property var leadingSlots: []
            readonly property var trailingSlots: [layoutTestMainSlotSize_trailing1]
            title.text: "Test"
            Item { id: layoutTestMainSlotSize_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(1); height: units.gu(3) }
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
            readonly property var leadingSlots: [layoutTestRelativePositioning2_leading1]
            readonly property var trailingSlots: [layoutTestRelativePositioning2_trailing2, layoutTestRelativePositioning2_trailing1]
            Item { id: layoutTestRelativePositioning2_leading1; SlotsLayout.position: SlotsLayout.Leading-3; width: units.gu(6); height: units.gu(3) }
            Item { id: layoutTestRelativePositioning2_trailing1; SlotsLayout.position: SlotsLayout.Trailing+1; width: units.gu(1); height: units.gu(7) }
            Item { id: layoutTestRelativePositioning2_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(9) }
        }
        ListItemLayout {
            id: layoutTestNullSizeSlots
            readonly property var leadingSlots: []
            property var trailingSlots: [layoutNullSizeSlots_trailing1, layoutNullSizeSlots_trailing2]
            title.text: "Hello"
            //we will reset the slot which is taller than mainSlot and test height and positions
            Item { id: layoutNullSizeSlots_trailing1; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(5); height: parent.mainSlot.height+20 }
            Item { id: layoutNullSizeSlots_trailing2; SlotsLayout.position: SlotsLayout.Trailing; width: units.gu(3); height: units.gu(4) }
        }
        ListItemLayout {
            id: layoutTestOverrideVerticalPositioning
            readonly property var leadingSlots: []
            readonly property var trailingSlots: [layoutTestOverrideVerticalPositioning_trailing1,
                layoutTestOverrideVerticalPositioning_trailing2]
            title.text: "Hello"
            //we will reset the slot which is taller than mainSlot and test height and positions
            Item {
                id: layoutTestOverrideVerticalPositioning_trailing1;
                SlotsLayout.position: SlotsLayout.Trailing;
                width: units.gu(5);
                height: units.gu(2)
            }
            Item {
                id: layoutTestOverrideVerticalPositioning_trailing2;
                SlotsLayout.overrideVerticalPositioning: true
                SlotsLayout.position: SlotsLayout.Trailing;
                anchors.bottom: parent.bottom
                width: units.gu(3);
                height: parent.mainSlot.height+10
            }
        }
        ListItemLayout {
            id: layoutTestIgnoreSomeChangesOnMainSlot
            readonly property var leadingSlots: [layoutTestIgnoreSomeChangesOnMainSlot_leading1]
            readonly property var trailingSlots: []
            title.text: "Hello"
            //we will reset the slot which is taller than mainSlot and test height and positions
            Item {
                id: layoutTestIgnoreSomeChangesOnMainSlot_leading1;
                SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(4)
                height: parent.mainSlot.height + 50
            }
        }
        ListItemLayout {
            id: layoutTestMainSlotAttachedProps
            readonly property var leadingSlots: [layoutTestMainSlotAttachedProps_leading1]
            readonly property var trailingSlots: []
            title.text: "Hello"
            //we will reset the slot which is taller than mainSlot and test height and positions
            Item {
                id: layoutTestMainSlotAttachedProps_leading1;
                SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(4)
                height: parent.mainSlot.height + 50
            }
        }
        ListItemLayout {
            id: layoutTestDefaultSlotsAttachedProps
            readonly property var leadingSlots: []
            property var trailingSlots: [layoutTestDefaultSlotsAttachedProps_trailing1]
            Item { id: layoutTestDefaultSlotsAttachedProps_trailing1 }
        }
        //UCLabel initially had REVISION 1 around textSize Q_PROPERTY
        //That breaks initialization of textSize from QML when done on
        //the UCLabels we created from C++! (changing textSize works from JS, fyi)
        //This component makes sure we don't break this usecase again in the future
        ListItemLayout {
            id: layoutTestCustomTextSizeInitializationFromQml
            title.text: "Hello"
            title.textSize: Label.XLarge
        }
    }

    UbuntuTestCase {
        name: "SlotsLayout"
        when: windowShown

        function warningMsg(msg) {
            return testUtil.callerFile() + ": " + msg
        }

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
                height = Math.max(height, item.mainSlot.height
                                  + item.mainSlot.SlotsLayout.padding.top
                                  + item.mainSlot.SlotsLayout.padding.bottom)
            }
            return height + item.padding.top + item.padding.bottom
        }

        //otherwise center the slots vertically
        function mustAlignSlotsToTop(item) {
            if (item.mainSlot === null) {
                return false
            }
            return maxSlotsHeight(item) < (item.mainSlot.height
                                           + item.mainSlot.SlotsLayout.padding.top
                                           + item.mainSlot.SlotsLayout.padding.bottom)
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

        function checkDefaultWrapping(item) {
            //we have to use WrapAnywhere because otherwise it will have unexpected behaviour
            //when used together with ElideRight when the string wraps (it will elide too early).
            compare(item.title.wrapMode, Label.WrapAnywhere, "Wrong default title wrapMode")
            compare(item.subtitle.wrapMode, Label.WrapAnywhere, "Wrong default subtitle wrapMode")
            compare(item.summary.wrapMode, Label.WrapAnywhere, "Wrong default summary wrapMode")
            compare(item.title.elide, Label.ElideRight, "Wrong default title elide")
            compare(item.subtitle.elide, Label.ElideRight, "Wrong default subtitle elide")
            compare(item.summary.elide, Label.ElideRight, "Wrong default summary elide")
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

                //mainSlot ignores the value of its overrideVerticalPositioning
                if (slot.SlotsLayout.overrideVerticalPositioning && slot !== item.mainSlot) {
                    //NOTE: we're assuming the test item doesn't set any custom anchor!!
                    compare(slot.y, 0, "Override vertical positioning: vertical position")
                } else {
                    if (mustAlignSlotsToTop(item)) {
                        compare(slot.anchors.top, item.top,
                                "Automatic vertical positioning: top anchor, \"aligned to the top\" positioning mode")
                        compare(slot.anchors.topMargin, item.padding.top + slot.SlotsLayout.padding.top,
                                "Automatic vertical positioning: topMargin, \"aligned to the top\" positioning mode")
                    } else {
                        compare(slot.anchors.verticalCenter, item.verticalCenter,
                                "Automatic vertical positioning: verticalCenter anchor, \"vertically centered\" positioning mode ")
                        compare(slot.anchors.verticalCenterOffset,
                                (item.padding.top - item.padding.bottom
                                 + slot.SlotsLayout.padding.top - slot.SlotsLayout.padding.bottom) / 2.0,
                                "Automatic vertical positioning: verticalCenterOffset, \"vertically centered\" positioning mode ")
                    }
                }
            }
        }

        function initTestCase() {
            // check the coloring of the mainslot labels
            compare(layoutEmpty.title.color, theme.palette.normal.backgroundText, "wrong title color");
            compare(layoutEmpty.subtitle.color, theme.palette.normal.backgroundSecondaryText, "wrong subtitle color");
            compare(layoutEmpty.summary.color, theme.palette.normal.backgroundTertiaryText, "wrong summary color");
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
            checkImplicitSize(layoutTestChangeSlotsSize)
        }

        function test_mainSlotSize() {
            compare(layoutTestMainSlotSize.mainSlot.width,
                    layoutTestMainSlotSize.width
                    - layoutTestMainSlotSize.padding.leading - layoutTestMainSlotSize.padding.trailing
                    - layoutTestMainSlotSize.trailingSlots[0].width
                    - layoutTestMainSlotSize.trailingSlots[0].SlotsLayout.padding.leading
                    - layoutTestMainSlotSize.trailingSlots[0].SlotsLayout.padding.trailing
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

        function test_nullSizeSlots() {
            compare(layoutTestNullSizeSlots.trailingSlots.length, 2, "Null size slot test, fake trailing slots list")
            checkSlotsPosition(layoutTestNullSizeSlots)
            checkImplicitSize(layoutTestNullSizeSlots)

            layoutTestNullSizeSlots.trailingSlots[0].height = 0
            compare(layoutTestNullSizeSlots.trailingSlots[0].height, 0, "Null size slot test, slot's height")
            //remove that slot from the list of slots we expect to be laid out
            var tmpSlot = layoutTestNullSizeSlots.trailingSlots[0]
            layoutTestNullSizeSlots.trailingSlots = [layoutTestNullSizeSlots.trailingSlots[1]]
            compare(layoutTestNullSizeSlots.trailingSlots.length, 1, "Null size slot test, fake trailing slots list")
            checkSlotsPosition(layoutTestNullSizeSlots)
            checkImplicitSize(layoutTestNullSizeSlots)

            layoutTestNullSizeSlots.trailingSlots[0].width = 0
            compare(layoutTestNullSizeSlots.trailingSlots[0].width, 0, "Null size slot test, slot's width")
            var tmpSlot2 = layoutTestNullSizeSlots.trailingSlots[0]
            layoutTestNullSizeSlots.trailingSlots = []
            compare(layoutTestNullSizeSlots.trailingSlots.length, 0, "Null size slot test, fake trailing slots list")
            checkSlotsPosition(layoutTestNullSizeSlots)
            checkImplicitSize(layoutTestNullSizeSlots)

            //bring the first slot back
            tmpSlot.height = 300
            compare(tmpSlot.height, 300, "Null size slot test, slot's height")
            layoutTestNullSizeSlots.trailingSlots = [tmpSlot]
            compare(layoutTestNullSizeSlots.trailingSlots.length, 1, "Null size slot test, fake trailing slots list")
            checkSlotsPosition(layoutTestNullSizeSlots)
            checkImplicitSize(layoutTestNullSizeSlots)

            //bring the second slot back
            tmpSlot2.width = 22
            compare(tmpSlot2.width, 22, "Null size slot test, slot's width")
            layoutTestNullSizeSlots.trailingSlots = [tmpSlot, tmpSlot2]
            compare(layoutTestNullSizeSlots.trailingSlots.length, 2, "Null size slot test, fake trailing slots list")
            checkSlotsPosition(layoutTestNullSizeSlots)
            checkImplicitSize(layoutTestNullSizeSlots)
        }

        function test_overrideVerticalPositioning() {
            //we expect the component to reset the vertical anchors when overrideVerticalPositioning changes value
            compare(layoutTestOverrideVerticalPositioning.trailingSlots[1].anchors.bottom,
                    layoutTestOverrideVerticalPositioning.bottom,
                    "Override vertical positioning test, initial slot's bottom anchor")
            compare(layoutTestOverrideVerticalPositioning.trailingSlots[1].SlotsLayout.overrideVerticalPositioning,
                    true, "Override vertical positioning test, attached-property's initial value")
            compare(layoutTestOverrideVerticalPositioning.trailingSlots.length, 2,
                    "Override vertical positioning test, fake trailing slots list")
            layoutTestOverrideVerticalPositioning.trailingSlots[1].SlotsLayout.overrideVerticalPositioning = false
            compare(layoutTestOverrideVerticalPositioning.trailingSlots[1].SlotsLayout.overrideVerticalPositioning,
                    false, "Override vertical positioning test, attached-property's new value, false")
            checkSlotsPosition(layoutTestOverrideVerticalPositioning)
            checkImplicitSize(layoutTestOverrideVerticalPositioning)

            layoutTestOverrideVerticalPositioning.trailingSlots[1].SlotsLayout.overrideVerticalPositioning = true
            compare(layoutTestOverrideVerticalPositioning.trailingSlots[1].SlotsLayout.overrideVerticalPositioning,
                    true, "Override vertical positioning test, attached-property's new value, true")
            checkSlotsPosition(layoutTestOverrideVerticalPositioning)
            checkImplicitSize(layoutTestOverrideVerticalPositioning)
        }

        function test_ignoreSomeChangesOnMainSlot() {
            compare(layoutTestIgnoreSomeChangesOnMainSlot.leadingSlots.length, 1,
                    "Ignore some changes on main slot test, fake trailing slots list")
            compare(layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.SlotsLayout.overrideVerticalPositioning,
                    false, "Ignore some changes on main slot test, default override value")
            //we want main slot to be vertically centered, to check if disabling overrideVerticalPositioning
            //will cause a change in its position
            compare(layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.height
                    < maxSlotsHeight(layoutTestIgnoreSomeChangesOnMainSlot), true,
                    "Ignore some changes on main slot test, slot's height")
            checkSlotsPosition(layoutTestIgnoreSomeChangesOnMainSlot)
            checkImplicitSize(layoutTestIgnoreSomeChangesOnMainSlot)

            //this shouldn't trigger a relayout
            layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.SlotsLayout.overrideVerticalPositioning = true
            compare(layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.SlotsLayout.overrideVerticalPositioning,
                    true, "Ignore some changes on main slot test, new override value")
            //let's force a relayout, just to make sure changing overrideVerticalPositioning
            //didn't have any effect on mainSlot
            layoutTestIgnoreSomeChangesOnMainSlot.leadingSlots[0].height += 100
            checkSlotsPosition(layoutTestIgnoreSomeChangesOnMainSlot)
            checkImplicitSize(layoutTestIgnoreSomeChangesOnMainSlot)

            //let's try to force positioning mainSlot *before* a leading slot. This must fail
            layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.SlotsLayout.position =
                    layoutTestIgnoreSomeChangesOnMainSlot.leadingSlots[0].SlotsLayout.position - 5
            compare(layoutTestIgnoreSomeChangesOnMainSlot.mainSlot.SlotsLayout.position,
                    layoutTestIgnoreSomeChangesOnMainSlot.leadingSlots[0].SlotsLayout.position - 5,
                    "Ignore some changes on main slot test, change mainSlot's position")
            checkSlotsPosition(layoutTestIgnoreSomeChangesOnMainSlot)
            checkImplicitSize(layoutTestIgnoreSomeChangesOnMainSlot)
        }

        function test_changeAttachedPropertiesInMainSlot() {
            checkSlotsPosition(layoutTestMainSlotAttachedProps)
            checkImplicitSize(layoutTestMainSlotAttachedProps)
            var mainSlot = layoutTestMainSlotAttachedProps.mainSlot

            mainSlot.SlotsLayout.padding.top = 300
            compare(layoutTestMainSlotAttachedProps.mainSlot.SlotsLayout.padding.top, 300,
                    "Change mainSlot's attached props test, padding.top")
            checkSlotsPosition(layoutTestMainSlotAttachedProps)
            checkImplicitSize(layoutTestMainSlotAttachedProps)

            mainSlot.SlotsLayout.padding.bottom = 180
            compare(mainSlot.SlotsLayout.padding.bottom, 180,
                    "Change mainSlot's attached props test, padding.bottom")
            checkSlotsPosition(layoutTestMainSlotAttachedProps)
            checkImplicitSize(layoutTestMainSlotAttachedProps)
        }

        function test_defaultSlotsAttachedProps() {
            var slot = layoutTestDefaultSlotsAttachedProps.trailingSlots[0]
            compare(slot.SlotsLayout.position, SlotsLayout.Trailing,
                    "Default slots attached props, position")
            compare(slot.SlotsLayout.overrideVerticalPositioning, false,
                    "Default slots attached props, overrideVerticalPositioning")
            compare(slot.SlotsLayout.padding.top, 0,
                    "Default slots attached props, padding.top")
            compare(slot.SlotsLayout.padding.bottom, 0,
                    "Default slots attached props, padding.bottom")
            compare(slot.SlotsLayout.padding.leading, units.gu(1),
                    "Default slots attached props, padding.leading")
            compare(slot.SlotsLayout.padding.trailing, units.gu(1),
                    "Default slots attached props, padding.trailing")
        }

        function checkLabelsY(listitemlayout) {
            compare(listitemlayout.title.y, 0,
                    "Default labels positioning, title's y")

            //we don't care about having the correct Y if the label
            //is empty or not visible anyway
            if (listitemlayout.subtitle.text !== ""
                    && listitemlayout.subtitle.visible) {
                if (listitemlayout.title.text === "" ||
                        !listitemlayout.title.visible) {
                    compare(listitemlayout.subtitle.y, 0,
                            "Default labels positioning, subtitle's y")
                } else {
                    compare(listitemlayout.subtitle.y,
                            listitemlayout.title.y + listitemlayout.title.height + units.dp(2),
                            "Default labels positioning, subtitle's y")
                }
            }

            //we don't care about having the correct Y if the label
            //is empty or not visible anyway
            if (listitemlayout.summary.text !== ""
                    && listitemlayout.summary.visible) {
                if (listitemlayout.subtitle.text === "" ||
                        !listitemlayout.subtitle.visible) {
                    if (listitemlayout.title.text === "" ||
                            !listitemlayout.title.visible) {
                        compare(listitemlayout.summary.y, 0,
                                "Default labels positioning, summary's y")
                    } else {
                        compare(listitemlayout.summary.y, listitemlayout.title.y + listitemlayout.title.height + units.dp(2),
                                "Default labels positioning, summary's y")
                    }
                } else  {
                    compare(listitemlayout.summary.y, listitemlayout.subtitle.y + listitemlayout.subtitle.height,
                            "Default labels positioning, summary's y")
                }
            }
        }

        function test_defaultLabelsWrappingAndElide() {
            checkDefaultWrapping(layoutLabels)
        }

        function test_defaultMainSlotHeight() {
            var titleText = layoutLabels.title.text
            var subtitleText = layoutLabels.subtitle.text
            var summaryText = layoutLabels.summary.text

            compare(titleText.length > 0, true, "Default labels positioning, title length")
            compare(subtitleText.length > 0, true, "Default labels positioning, subtitle length")
            compare(summaryText.length > 0, true, "Default labels positioning, summary length")
            checkLabelsY(layoutLabels)

            var initialHeight = layoutLabels.mainSlot.height
            compare(initialHeight, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, mainSlot's height")

            //setting text to " " (note, it's NOT empty) should have the same effect as setting it to "blabla"
            layoutLabels.summary.text = " "
            compare(layoutLabels.summary.text, " ", "Default labels positioning, summary 1 whitespace text")
            compare(layoutLabels.mainSlot.height, initialHeight,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            //empty text -> we ignore summary in the height computation
            layoutLabels.summary.text = ""
            compare(layoutLabels.summary.text, "", "Default labels positioning, empty summary text")
            compare(layoutLabels.mainSlot.height, layoutLabels.subtitle.y + layoutLabels.subtitle.height,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            layoutLabels.subtitle.text = ""
            compare(layoutLabels.subtitle.text, "", "Default labels positioning, empty subtitle text")
            compare(layoutLabels.mainSlot.height, layoutLabels.title.y + layoutLabels.title.height,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            //all labels are empty now
            layoutLabels.title.text = ""
            compare(layoutLabels.title.text, "", "Default labels positioning, empty title text")
            compare(layoutLabels.subtitle.text, "", "Default labels positioning, empty subtitle text")
            compare(layoutLabels.summary.text, "", "Default labels positioning, empty summary text")
            compare(layoutLabels.mainSlot.height, 0,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            //try title and summary, skipping subtitle
            layoutLabels.title.text = "test"
            layoutLabels.summary.text = "test"
            compare(layoutLabels.title.text, "test", "Default labels positioning, empty title text")
            compare(layoutLabels.subtitle.text, "", "Default labels positioning, empty subtitle text")
            compare(layoutLabels.summary.text, "test", "Default labels positioning, empty summary text")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            //let's try with summary only
            layoutLabels.title.text = ""
            compare(layoutLabels.title.text, "", "Default labels positioning, empty title text")
            compare(layoutLabels.subtitle.text, "", "Default labels positioning, empty subtitle text")
            compare(layoutLabels.summary.text, "test", "Default labels positioning, empty summary text")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)

            //try to set all texts and test positions again
            layoutLabels.title.text = titleText
            layoutLabels.subtitle.text = subtitleText
            layoutLabels.summary.text = summaryText
            compare(layoutLabels.mainSlot.height, initialHeight,
                    "Default labels positioning, mainSlot's height")
            checkLabelsY(layoutLabels)
        }

        function test_fontsizeChange() {
            var initialHeight = layoutLabels.mainSlot.height
            compare(layoutLabels.title.textSize, Label.Medium, "Labels textSize change, default title textsize")
            compare(layoutLabels.subtitle.textSize, Label.Small, "Labels textSize change, default subtitle textsize")
            compare(layoutLabels.summary.textSize, Label.Small, "Labels textSize change, default summary textsize")

            compare(layoutLabels.title.text.length > 0, true, "Default labels positioning, title length")
            compare(layoutLabels.subtitle.text.length > 0, true, "Default labels positioning, subtitle length")
            compare(layoutLabels.summary.text.length > 0, true, "Default labels positioning, summary length")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, main slot height")

            layoutLabels.title.textSize = Label.XLarge
            compare(layoutLabels.title.textSize, Label.XLarge, "Labels textSize change, new title textsize")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, main slot height")

            layoutLabels.subtitle.textSize = Label.XLarge
            compare(layoutLabels.subtitle.textSize, Label.XLarge, "Labels textSize change, new subtitle textsize")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, main slot height")

            layoutLabels.summary.textSize = Label.XLarge
            compare(layoutLabels.summary.textSize, Label.XLarge, "Labels textSize change, new summary textsize")
            compare(layoutLabels.mainSlot.height, layoutLabels.summary.y + layoutLabels.summary.height,
                    "Default labels positioning, main slot height")
        }

        Label {id: customMainSlot }
        function test_warningOnAttemptToChangeListItemLayoutMainSlot() {
            ignoreWarning(warningFormat(60, 9, "QML ListItemLayout: Setting a different mainSlot on ListItemLayout is not supported. Please use SlotsLayout instead."))
            layoutLabels.mainSlot = customMainSlot
        }


        Component {
            id: layoutTestQmlContextComponent
            ListItemLayout {
                id: layoutTestQmlContext
                title.text: "<html><body><p dir='ltr'>TEST <img align=absmiddle height=\"10\" width=\"10\" src=\"file:///test.png\" /> </p></body></html>"
                title.textFormat: Text.RichText
                subtitle.text: "<html><body><p dir='ltr'>TEST <img align=absmiddle height=\"10\" width=\"10\" src=\"file:///test.png\" /> </p></body></html>"
                subtitle.textFormat: Text.RichText
                summary.text: "<html><body><p dir='ltr'>TEST <img align=absmiddle height=\"10\" width=\"10\" src=\"file:///test.png\" /> </p></body></html>"
                summary.textFormat: Text.RichText
            }
        }
        //lp#1514173
        //this will make the test segfault if there is a regression
        function test_defaultLabelsQmlContext() {
            ignoreWarning(warningMsg("QML Label: Cannot open: file:///test.png"))
            var obj = layoutTestQmlContextComponent.createObject(main)
            //wait for rendering otherwise we will not get the "cannot find file" warning
            //because the img is loaded async
            waitForRendering(obj)
            compare(obj !== null, true, "QML ListItemLayout: testing labels' QML context.")
            obj.destroy()
        }

        //first version of ListItemLayout anchored subtitle to title's baseline, but that breaks
        //when title is multiline
        function test_multilineLabelsPositioning() {
            compare(layoutMultilineLabels.title.maximumLineCount, 2,
                    "Multiline labels positioning: wrong title maximumLineCount")
            compare(layoutMultilineLabels.subtitle.maximumLineCount, 3,
                    "Multiline labels positioning: wrong subtitle maximumLineCount")
            compare(layoutMultilineLabels.summary.maximumLineCount, 2,
                    "Multiline labels positioning: wrong summary maximumLineCount")

            compare(layoutMultilineLabels.title.lineCount, 2,
                    "Multiline labels positioning: wrong title lineCount")
            compare(layoutMultilineLabels.subtitle.lineCount, 3,
                    "Multiline labels positioning: wrong subtitle lineCount")
            compare(layoutMultilineLabels.summary.lineCount, 2,
                    "Multiline labels positioning: wrong summary lineCount")
            checkLabelsY(layoutMultilineLabels)
        }
    }
}
