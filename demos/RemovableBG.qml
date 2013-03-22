import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    id: __backgroundItem

    anchors.fill: parent
    color: "gray"
    Label {
        id: __backgroundText

        property int slidingMargin: units.gu(3)

        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        text: "Clear"
        fontSize: "medium"
        color: "#e8e1d0"
        font.bold: true
    }

    states: [
        State {
            name: "SwipingRight"
            PropertyChanges {
                target: __backgroundText
                anchors.rightMargin: __backgroundText.slidingMargin
                anchors.leftMargin: 0
                horizontalAlignment: Text.AlignRight

            }
        },
        State {
            name: "SwipingLeft"
            PropertyChanges {
                target: __backgroundText
                anchors.rightMargin: 0
                anchors.leftMargin: __backgroundText.slidingMargin
                horizontalAlignment: Text.AlignLeft
            }
        }
    ]
}
