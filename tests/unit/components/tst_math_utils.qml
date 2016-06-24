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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1

TestCase {
    name: "MathUtils"

    function test_clamp_positive_lower() {
        var minValue = 9
        var maxValue = 42
        var clampValue = -7

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, minValue, "clamped value not within range")
    }

    function test_clamp_positive_greater() {
        var minValue = 9
        var maxValue = 42
        var clampValue = 111

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, maxValue, "clamped value not within range")
    }

    function test_clamp_positive_within() {
        var minValue = 9
        var maxValue = 53
        var clampValue = 42

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, clampValue, "clamped value changed even though it shouldn't have")
    }

    function test_clamp_positive_on_border() {
        var minValue = 9
        var maxValue = 42
        var clampValue = 9

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, clampValue, "clamped value changed even though it shouldn't have")
    }

    function test_clamp_negative_lower() {
        var minValue = -42
        var maxValue = -9
        var clampValue = -50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, minValue, "clamped value not within range")
    }

    function test_clamp_negative_greater() {
        var minValue = -42
        var maxValue = -9
        var clampValue = 50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, maxValue, "clamped value not within range")
    }

    function test_clamp_postive_and_negative_greater() {
        var minValue = -42
        var maxValue = 9
        var clampValue = 50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, maxValue, "clamped value not within range")
    }

    function test_clamp_positive_lower_switched() {
        var minValue = 42
        var maxValue = 9
        var clampValue = -7

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, maxValue, "clamped value not within range")
    }

    function test_clamp_positive_greater_switched() {
        var minValue = 42
        var maxValue = 9
        var clampValue = 111

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, minValue, "clamped value not within range")
    }

    function test_clamp_positive_within_switched() {
        var minValue = 53
        var maxValue = 9
        var clampValue = 42

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, clampValue, "clamped value changed even though it shouldn't have")
    }

    function test_clamp_positive_on_border_switched() {
        var minValue = 42
        var maxValue = 9
        var clampValue = 9

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, clampValue, "clamped value changed even though it shouldn't have")
    }

    function test_clamp_negative_lower_switched() {
        var minValue = -9
        var maxValue = -42
        var clampValue = -50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, maxValue, "clamped value not within range")
    }

    function test_clamp_negative_greater_switched() {
        var minValue = -9
        var maxValue = -42
        var clampValue = 50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, minValue, "clamped value not within range")
    }

    function test_clamp_postive_and_negative_greater_switched() {
        var minValue = 9
        var maxValue = -42
        var clampValue = 50

        var clamped = MathUtils.clamp(clampValue, minValue, maxValue)
        compare(clamped, minValue, "clamped value not within range")
    }

    function test_lerp() {
        var lerped = MathUtils.lerp(0.25, 90, 0)
        compare(lerped, 67.5)
    }

    function test_project_value() {
        var projectedValue = MathUtils.projectValue(5, 1, 100, 2, 200)
        compare(projectedValue, 10)
    }

    function test_clamp_and_project() {
        var clampedAndProjectedValue = MathUtils.clampAndProject(5, 1, 10, 2, 200)
        compare(clampedAndProjectedValue, 90)
    }
}
