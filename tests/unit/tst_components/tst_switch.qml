import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "SwitchAPI"

     function test_checked() {
         compare(switchComponent.checked,false,"checked is false by default")

         switchComponent.checked = true
         compare(switchComponent.checked,true,"Can set/get checked")
     }

     function test_hovered() {
        compare(switchComponent.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(switchComponent.pressed,false,"Pressed is boolean and false by default")
     }

     Switch {
         id: switchComponent
     }
}
