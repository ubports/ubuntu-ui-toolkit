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

    function test_fontSize() {
        compare(textCustom.fontSize,"medium","fontSize is 'medium' by default")

        var fontSizes = ["xx-small", "x-small", "small", "medium", "large", "x-large", "xx-large" ]

        for (var i=0;i<fontSizes.length;i++)
        {

            var newFontSize = fontSizes[i]
            console.debug("Testing with fontSize " + newFontSize)

            textCustom.fontSize = newFontSize

            try {
                expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076771")
                compare(textCustom.newFontSize,newFontSize,"Can set/get " + newFontSize)
            } catch(err) {
                console.debug("Found a bug, continuing for the other items in list..")
            }
        }
    }

    function test_fontWeight() {
        compare(textCustom.font.weight, Font.Normal, "font.weight is 'normal' by default")

        var fontWeights = [ Font.Light, Font.Normal, Font.DemiBold, Font.Bold, Font.Black ]

        for (var i in fontWeights)
        {
            var newFontWeight = fontWeights[i]
            textCustom.font.weight = newFontWeight

            compare(textCustom.font.weight, newFontWeight, "can set/get " + newFontWeight)
        }
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
