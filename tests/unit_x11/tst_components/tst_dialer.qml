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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0

Item {
    id: root
    width: 200
    height: 200

    Dialer {
        id: noHands
    }
    Dialer {
        id: oneHand
        DialerHand {
            id: hand
        }
    }
    Dialer {
        DialerHand {
            id: overlayHand
            Label {
                id: overlayItem
                text: overlayHand.value
            }
        }
    }

    SignalSpy {
        id: spy
    }

    TestCase {
        id: testCase
        name: "DialerAPI"
        when: windowShown

        function test_0_Dialer_minimumValue() {
            compare(noHands.minimumValue, 0.0, "noHands.minimumValue");
            compare(oneHand.minimumValue, 0.0, "oneHand.minimumValue");
        }

        function test_0_Dialer_maximumValue() {
            compare(noHands.maximumValue, 360.0, "noHands.maximumValue");
            compare(oneHand.maximumValue, 360.0, "oneHand.maximumValue");
        }

        function test_0_Dialer_size() {
            compare(noHands.size, units.gu(32), "noHands.size");
            compare(oneHand.size, units.gu(32), "oneHand.size");
        }

        function test_0_Dialer_handSpace() {
            compare(noHands.handSpace, units.gu(6.5), "noHands.handSpace");
            compare(oneHand.handSpace, units.gu(6.5), "oneHand.handSpace");
        }

        function test_0_Dialer_centerItem() {
            verify(noHands.centerItem !== undefined, "noHands.centerItem");
            verify(oneHand.centerItem !== undefined, "oneHand.centerItem");
        }

        function test_0_Dialer_centerContent() {
            verify(noHands.centerContent !== undefined, "noHands.centerContent");
            verify(oneHand.centerContent !== undefined, "oneHand.centerContent");
        }

        function test_0_Dialer_hands() {
            verify(noHands.hands !== undefined, "hoHands.hands is defined");
            compare(noHands.hands.length, 0, "noHands.hands.length");
            compare(oneHand.hands.length, 1, "noHands.hands.length");
            compare(oneHand.hands[0], hand, "noHands.hands[0] == hand");
        }

        function test_0_DialerHand_value() {
            compare(hand.value, 0.0, "hand.value");
        }

        function test_0_DialerHand_hand_height() {
            verify(hand.hand.height !== 0.0, "hand.hand.height");
        }

        function test_0_DialerHand_hand_width() {
            verify(hand.hand.width !== 0.0, "hand.hand.width");
        }

        function test_0_DialerHand_hand_draggable() {
            compare(hand.hand.draggable, true, "hand.hand.draggable");
        }

        function test_0_DialerHand_hand_visible() {
            compare(hand.hand.visible, true, "hand.hand.visible");
        }

        function test_0_DialerHand_hand_toCenterItem() {
            compare(hand.hand.toCenterItem, false, "hand.hand.toCenterItem");
        }

        function test_0_DialerHand_index() {
            compare(hand.index, 0, "hand.index");
        }

        function test_0_DialerHand_dialer() {
            compare(hand.dialer, oneHand, "hand.dialer");
        }

        function test_0_DialerHand_overlay() {
            verify(hand.overlay !== undefined, "hand.overlay");
        }

        function test_DialerHand_valueChange() {
            spy.clear();
            spy.signalName = "onHandUpdated";
            spy.target = oneHand;
            hand.value = 100;
            compare(hand.value, 100, "hand.value changed");
            tryCompare(spy, "count", 1);
        }

        function test_DialerHand_hand_toCenterItem_Change() {
            hand.hand.toCenterItem = true;
            compare(hand.hand.toCenterItem, true, "hand.hand.toCenterItem changed");
        }

        function test_DialerHand_hand_draggable_Change() {
            hand.hand.draggable = false;
            compare(hand.hand.draggable, false, "hand.hand.draggable changed");
        }

        function test_DialerHand_handSizeChange() {
            var prevHeight = hand.hand.height;
            hand.hand.height = units.gu(10);
            verify(hand.hand.height !== prevHeight, "hand.hand.height");
            compare(hand.hand.height, units.gu(10), "hand.hand.height changed");

            var prevWidth = hand.hand.width;
            hand.hand.width = units.gu(10);
            verify(hand.hand.width !== prevWidth, "hand.hand.width");
            compare(hand.hand.width, units.gu(10), "hand.hand.width changed");
        }

        function test_OverlayHand_visible() {
            overlayHand.hand.visible = false;
            compare(overlayItem.visible, true, "overLayItem.visible");
        }

        function test_OverlayHand_rotation() {
            overlayHand.value = 50;
            compare(overlayItem.rotation, 0.0, "overLayItem.rotation");
        }

        function test_OverlayHand_valueChange() {
            overlayHand.value = 70;
            compare(overlayItem.text, "70", "overLayItem.text");
        }
    }
}
