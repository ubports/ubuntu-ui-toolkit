import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "SliderAPI"

     function test_pressed() {
        compare(slider.pressed,false,"Pressed is boolean and false by default")
     }

     function test_hovered() {
        compare(slider.hovered,false,"Hovered is boolean and false by default")
     }

     function test_live() {
        compare(slider.live,false,"Live is boolean and false by default")
         var newLive = true
         slider.live = newLive
         compare(slider.live,newLive,"can set/get")
     }

     function test_maximumValue() {
         compare(slider.maximumValue,1.0,"is set to 1.0 by default")
         var newMaximumValue = 20.0
         slider.maximumValue = newMaximumValue
         compare(slider.maximumValue,newMaximumValue,"can set/get")
     }

     function test_minimumValue() {
         compare(slider.minimumValue,0,"is set to 0 by default")
         var newMinimumValue = -20.0
         slider.minimumValue = newMinimumValue
         compare(slider.minimumValue,newMinimumValue,"can set/get")
     }

     function test_value() {
         compare(slider.value,0,"is set to 0 by default")
         var newValue = 5
         slider.value = newValue
         compare(slider.value,newValue,"can set/get")
     }

     Slider {
         id: slider
     }
}
