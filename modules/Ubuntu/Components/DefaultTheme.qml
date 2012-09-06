// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    ButtonStyle {
        states: [""]
        color: "#e3e5e8"
        property bool darkBorder: false
        borderShape: darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
        borderImage: darkBorder ? "artwork/ButtonBorderDarkIdle.png" : "artwork/ButtonBorderIdle.png"
    }
    ButtonStyle {
        states: ["pressed"]
        color: "pink"
        property bool darkBorder: false
        borderShape: darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
        borderImage: darkBorder ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
    }
}
