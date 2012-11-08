import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "AbstractButtonAPI"

     function test_hovered() {
        compare(absButton.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(absButton.pressed,false,"Pressed is boolean and false by default")
     }

     AbstractButton {
         id: absButton
     }
}
