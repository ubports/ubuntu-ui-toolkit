import Qt 4.7
import Ubuntu.Components 0.1

Rectangle {
  width: 640
  height: 480

  Column {  
    TextCustom {
      text: "Ubuntu Sans"
      font: UiConstants.BodyTextFont
    }

    TextCustom {
      text: "Ubuntu Sans Light"
      font: UiConstants.HeaderFont
    }
  }
}
