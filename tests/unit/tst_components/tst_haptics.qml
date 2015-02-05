/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.3
import QtTest 1.0
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0
import QtFeedback 5.0

UbuntuTestCase {
    name: "HapticsAPI"

    function waitForHapticsCompleted() {
        tryCompareFunction(function() { return Haptics.effect.state; }, HapticsEffect.Stopped, 1000);
    }

    function test_0_defaults() {
        verify(Haptics.hasOwnProperty("enabled"), "missing property 'enabled'");
        verify(Haptics.hasOwnProperty("effect"), "missing property 'effect'");
        verify(Haptics.hasOwnProperty("play"), "missing function 'play'");
    }

    function test_play() {
        Haptics.play();
        if (Haptics.enabled) {
            waitForHapticsCompleted();
        }
    }

    function test_custom_play() {
        Haptics.play({attackTime: 10, attackIntensity: 0.5, duration: 1200});
        if (Haptics.enabled && Haptics.effect.running) {
            compare(Haptics.effect.attackTime, 10, "attack time not modified");
            compare(Haptics.effect.attackIntensity, 0.5, "attack intensity not modified");
            compare(Haptics.effect.duration, 400, "duration not modified");
            waitForHapticsCompleted();
        }
    }
}
