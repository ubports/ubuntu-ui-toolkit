import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: root;
    property int count: 2;

    property real t;
    NumberAnimation on t { from: 0; to: 1; duration: 1000; loops: Animation.Infinite }
    onTChanged: {
        repeater.model = 0;
        repeater.model = root.count
    }

    Component.onCompleted: repeater.model = root.count

    Repeater {
        id: repeater
        ScrollbarCpp {
            x: Math.random() * (root.width - width)
            y: Math.random() * (root.height - height)
            width: 20
            height: 200
        }
    }
}

