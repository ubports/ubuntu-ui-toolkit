    import QtQuick 2.0
    import Ubuntu.Components 0.1

Column {
    Slider {
       id: slider
       minimumValue: 0
       maximumValue: 11
    }
Button {
            text: "live"
            onClicked: {
                slider.live = !slider.live;
            }
        }
        Button {
            text: "minmax_values"
            onClicked: {
                slider.minimumValue = slider.minimumValue + 1;
                slider.maximumValue = slider.maximumValue + 10;
            }
        }
        Button {
            text: "reset values"
            onClicked: {
                slider.value = -10;
                slider.minimumValue = -10;
                slider.maximumValue = 10;
                timer.running = false;
            }
        }
        Button {
            text: "inc value"

            property bool running : timer.running

            onClicked: {
                timer.running = true;
            }
        }
        Timer {
            id: timer
            interval: 100
            running: false
            repeat: true
            onTriggered: {
                slider.value += 1;
                running = (slider.value!==slider.maximumValue) 
            }
        }
}
