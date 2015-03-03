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
import Ubuntu.Components 1.1

TestCase {
    name: "LabelAPI"

    function cleanup() {
        textCustom.fontSize = "medium";
        textCustom.font.weight = Font.Light;
    }

    function test_0_defaults() {
        compare(textCustom.font.family, "Ubuntu", "Default font family");
        compare(textCustom.font.weight, Font.Light, "Default font weight");
        compare(textCustom.fontSize,"medium","fontSize is 'medium' by default")
    }

    function test_fontSize_data() {
        return [
            {tag: "xx-small"},
            {tag: "x-small"},
            {tag: "small"},
            {tag: "medium"},
            {tag: "large"},
            {tag: "x-large"},
        ];
    }
    function test_fontSize(data) {
        textCustom.fontSize = data.tag;
        compare(textCustom.fontSize, data.tag, "Can set/get " + data.tag)
    }

    function test_fontWeight_data() {
        return [
            {tag: "Light", weight: Font.Light},
            {tag: "Normal", weight: Font.Normal},
            {tag: "DemiBold", weight: Font.DemiBold},
            {tag: "Bold", weight: Font.Bold},
            {tag: "Black", weight: Font.Black},
        ];
    }
    function test_fontWeight(data) {
        textCustom.font.weight = data.weight
        compare(textCustom.font.weight, data.weight, "can set/get " + data.weight);
    }

    function test_boldWeightConflict() {
        compare(lightLabel.font.weight, Font.Light, "font.weight is not overriden by font.bold")
    }

    function test_weightPrecedence() {
        compare(lightLabel2.font.weight, Font.Light, "font.weight takes precedence over font.bold")
    }

    Label {
        id: textCustom
    }

    Label {
        id: lightLabel
        font.weight: Font.Light
    }

    Label {
        id: lightLabel2
        font.weight: Font.Light
        font.bold: true
    }
}
