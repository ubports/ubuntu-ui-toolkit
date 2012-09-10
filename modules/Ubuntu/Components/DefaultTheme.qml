// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Ubuntu.Components 0.1

Item {
    ButtonStyle {
        states: [""]
        color: "#e3e5e8"
        property bool dborder: false
        borderShape: dborder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
        borderImage: dborder ? "artwork/ButtonBorderDarkIdle.png" : "artwork/ButtonBorderIdle.png"
    }
    ButtonStyle {
        states: ["pressed"]
        color: "pink"
        property bool dborder: false

        Connections {
            target: button

            onDarkBorderChanged: {
                console.debug("dark border")
            }
            ignoreUnknownSignals: true
        }

        borderShape: dborder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
        borderImage: dborder ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
    }
}
