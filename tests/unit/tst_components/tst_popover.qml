import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

TestCase {
     name: "PopoverAPI"

     function test_show() {
         //TODO: does not work?
         // Uncaught exception: Object [object Object] has no method &apos;show&apos;
         PopupUtils.show(popoverComponent,rootItem);
     }

     function test_close() {
         //TODO
     }

     Rectangle {
         id: rootItem
     }

     Component {
         id: popoverComponent
         Popover {
             id: popOver
             Text {
                 text: "Hello Popover!"
             }
         }
     }
}
