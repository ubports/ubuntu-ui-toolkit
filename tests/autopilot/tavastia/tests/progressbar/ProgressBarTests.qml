import QtQuick 2.0
    import Ubuntu.Components 0.1

    Column {
        ProgressBar {
           id: progressBar
           minimumValue: 1
           maximumValue: 10
        }
        Button {
            text: "indeterminate"
            onClicked: {
                progressBar.indeterminate = !progressBar.indeterminate;
            }
        }
        Button {
            text: "minmax_values"
            onClicked: {
                progressBar.minimumValue = progressBar.minimumValue + 1;
                progressBar.maximumValue = progressBar.maximumValue + 10;
            }
        }
        Button {
            text: "reset values"
            onClicked: {
                progressBar.value = -10;
                progressBar.minimumValue = -10;
                progressBar.maximumValue = 10;
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
                progressBar.value += 1;
                running = (progressBar.value!==progressBar.maximumValue) 
            }
        }
    }
