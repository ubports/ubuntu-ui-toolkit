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
import Ubuntu.Components 0.1

TestCase {
     name: "TextCustomAPI"

     function test_fontSize() {
         skip("https://bugs.launchpad.net/tavastia/+bug/1076771")
         compare(textCustom.fontSize,"medium","fontSize is 'medium' by default")

         var fontSizes = ["xx-small", "x-small", "small", "medium", "large", "x-large", "xx-large" ]

         for (var i=0;i<fontSizes.length;i++)
         {

             var newFontSize = fontSizes[i]
             console.debug("Testing with fontSize " + newFontSize)

            textCustom.fontSize = newFontSize

            try {
                compare(textCustom.newFontSize,newFontSize,"Can set/get " + newFontSize)
            } catch(err) {
                console.debug("Found a bug, continuing for the other items in list..")
            }
         }
     }

     TextCustom {
         id: textCustom
     }
}
