import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "ButtonAPI"

     function test_text() {
         compare(button.text,"","Button does not have text set by default")
         var newText = "Hello World!"
         button.text = newText
         compare(button.text,newText,"Can set/get text")
     }

     function test_iconPosition() {
        compare(button.iconPosition,"left","The default value for iconPosition is 'left'")
         var newIconPosition = "right"
         button.iconPosition = newIconPosition
         compare(button.iconPosition,newIconPosition)
     }

     function test_color() {
        var newColor = "#f00baa"
        button.color = newColor
        compare(button.color,newColor,"Can set/get color")
     }

     function test_hovered() {
        compare(button.hovered,false,"Hovered is boolean and false by default")
     }

     function test_iconSource() {
        compare(button.iconSource,"","iconSource is not set by default")
        var newIcon = "../../../demos/small_avatar.png"
        button.iconSource = newIcon
        compare(button.iconSource,Qt.resolvedUrl(newIcon),"Can set/get iconSource")
     }

     function test_pressed() {
        compare(button.pressed,false,"Pressed is boolean and false by default")
     }

     Button {
         id: button
     }
}
