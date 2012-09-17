// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Ubuntu.Components 0.1

Item {

    Style {
        styleClass: "Button"
        //selector: ".Button"
        style: ButtonStyle {
            color: control.pressed ? "purple" : control.hovered ? "cyan" : "pink"//"#e3e5e8"
            borderShape: control.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
            borderImage: if (control.darkBorder) return control.pressed ? "artwork/ButtonBorderDarkPressed.png" : "artwork/ButtonBorderDarkIdle.png"
                    else return control.pressed ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
        }
    }

    // sample for hierarchy
    Style {
        styleClass: ".Toolbar"
        Style {
            styleClass: ".TheButton"
            instanceId: "quit"
        }
    }
}
