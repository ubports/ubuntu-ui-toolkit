import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "CheckBoxAPI"

     function test_checked() {
         compare(checkbox.checked,false,"CheckBox.checked is false by default")

         checkbox.checked = true
         compare(checkbox.checked,true,"Can set/get checked")
     }

     function test_hovered() {
        compare(checkbox.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(checkbox.pressed,false,"Pressed is boolean and false by default")
     }

     CheckBox {
         id: checkbox
     }
}
