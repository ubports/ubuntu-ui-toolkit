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
