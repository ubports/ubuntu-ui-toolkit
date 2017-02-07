
import QtQuick 2.4
import Ubuntu.Components 1.3

Repeater {
    width: 400
    height: 600

    model: 100
    delegate: Image {
        source: "face.png"
    }
}
