import QtQuick 2.0
import Ubuntu.Components 0.1

Column {
    ActivityIndicator {
       id: activityIndicator
    }
    Button {
        text: "Toggle Indicator"
        onClicked: {
            activityIndicator.running = !activityIndicator.running
        }
    }
}
