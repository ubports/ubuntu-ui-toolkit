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
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Components 1.3

TestCase {
    name: "Label13API"

    function cleanup() {
        textCustom.fontSize = "medium";
        textCustom.font.weight = Font.Light;
    }

    function initTestCase() {
        compare(textCustom.font.family, "Ubuntu", "Default font family");
        compare(textCustom.font.weight, Font.Light, "Default font weight");
        compare(textCustom.fontSize, "medium", "fontSize is 'medium' by default")
        compare(textCustom.textSize, Label.Medium, "fontSize is 'medium' by default")
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

    function test_textSize_data() {
        return [
            {tag: "XxSmall", size: Label.XxSmall},
            {tag: "XSmall", size: Label.XSmall},
            {tag: "Small", size: Label.Small},
            {tag: "Medium", size: Label.Medium},
            {tag: "Large", size: Label.Large},
            {tag: "XLarge", size: Label.XLarge},
        ];
    }
    function test_textSize(data) {
        textCustom.textSize = data.tag;
        compare(textCustom.textSize, data.size, "Can set/get " + data.tag)
    }

    // this must be executed on a Label which is tested only by this function
    function test_textSize_suppresses_fontSize() {
        textSizeTest.textSize = Label.XLarge;
        textSizeTest.fontSize = "xx-small";
        compare(textSizeTest.textSize, Label.XLarge, "fontSize overruled textSize!");
    }

    function test_fontSize_equals_textSize_data() {
        return [
            {tag: "xx-small", size: Label.XxSmall},
            {tag: "x-small", size: Label.XSmall},
            {tag: "small", size: Label.Small},
            {tag: "medium", size: Label.Medium},
            {tag: "large", size: Label.Large},
            {tag: "x-large", size: Label.XLarge},
        ];
    }
    function test_fontSize_equals_textSize(data) {
        textFontSize.textSize = data.size;
        fuzzyCompare(textFontSize.font.pixelSize, FontUtils.sizeToPixels(data.tag), 0.999, "pixelSize differs for " + data.tag);
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


    function verifyAutomaticRenderType(label) {
        if (units.gridUnit <= 10) {
            compare(label.renderType, Text.NativeRendering,
                    "On low dpi screen renderType is Text.NativeRendering by default");
        } else {
            compare(label.renderType, Text.QtRendering,
                    "On high dpi screen renderType is Text.QtRendering by default");
        }
    }

    function verifyManualRenderType(label, renderType) {
        compare(label.renderType, renderType,
                "renderType was set manually, no automatic change should happen to it");
    }

    function test_renderTypeDefault_data() {
        return [
            {tag: "Default", gridUnit: 8.0},
            {tag: "HighDPI", gridUnit: 16.0},
            {tag: "LowDPI", gridUnit: 10.0},
        ];
    }

    function test_renderTypeDefault(data) {
        verifyAutomaticRenderType(textRenderTypeDefault);
        units.gridUnit = data.gridUnit;
        verifyAutomaticRenderType(textRenderTypeDefault);
    }

    function test_presetRenderType_data() {
        return test_renderTypeDefault_data();
    }

    function test_setRenderType() {
        units.gridUnit = 8;
        verifyAutomaticRenderType(textSetRenderType);
        units.gridUnit = 16;
        verifyAutomaticRenderType(textSetRenderType);

        // set renderType manually
        textSetRenderType.renderType = Text.NativeRendering;
        verifyManualRenderType(textSetRenderType, Text.NativeRendering);

        units.gridUnit = 8;
        verifyManualRenderType(textSetRenderType, Text.NativeRendering);

        units.gridUnit = 16;
        verifyManualRenderType(textSetRenderType, Text.NativeRendering);
    }

    Label {
        id: textCustom
    }
    Label {
        id: textFontSize
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

    Label {
        id: textSizeTest
    }

    Label {
        id: textRenderTypeDefault
    }

    Label {
        id: textRenderTypePreset
        renderType: Text.QtRendering
    }

    Label {
        id: textSetRenderType
    }
}
