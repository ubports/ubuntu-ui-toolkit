// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Ubuntu.Components 0.1

Item {
    ButtonStyle {
        styleClass: "DarkBorder"
        color: "pink"//"#e3e5e8"
        borderShape: "artwork/ButtonShapeDark.png"
        borderImage: "artwork/ButtonBorderDarkIdle.png"
        states: ["hovered", ""]
    }
    ButtonStyle {
        styleClass: "DarkBorder"
        color: "purple"//"#e3e5e8"
        borderShape: "artwork/ButtonShapeDark.png"
        borderImage: "artwork/ButtonBorderDarkIdle.png"
        states: ["pressed"]
    }

    ButtonStyle {
        color: "pink"//"#e3e5e8"
        borderShape: "artwork/ButtonShape.png"
        borderImage: "artwork/ButtonBorderIdle.png"
        states: ["hovered", ""]
    }
    ButtonStyle {
        color: "purple"//"#e3e5e8"
        borderShape: "artwork/ButtonShape.png"
        borderImage: "artwork/ButtonBorderIdle.png"
        states: ["pressed"]
    }
    ButtonStyle {
        instanceId: "callButton"
        color: "purple"

        borderShape: target.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
        borderImage: target.darkBorder ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
    }
}
