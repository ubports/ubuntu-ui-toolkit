import QtQuick 2.4
import Ubuntu.Components 1.3

SlotsLayoutCpp {
    property alias title: titleLabel
    property alias subtitle: subtitleLabel
    property alias summary: summaryLabel

    mainSlot: Item {
        //even if a Label is empty, its height will still be "lineHeight" value,
        //and that ruins the layout
        height: childrenRect.height

        Label {
            id: titleLabel
            width: parent.width
            height: text != "" ? contentHeight : 0
            font.weight: Font.Light
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 1
        }
        Label {
            id: subtitleLabel
            width: parent.width
            height: text != "" ? contentHeight : 0
            anchors {
                top: titleLabel.text != "" ? titleLabel.baseline : titleLabel.top;
                topMargin: (text != "" && titleLabel.text != "") ? units.gu(1) : 0
            }
            fontSize: "small"
            font.weight: Font.Light
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 1
        }
        Label {
            id: summaryLabel
            width: parent.width
            height: text != "" ? contentHeight : 0
            anchors {
                top: subtitleLabel.text != "" ? subtitleLabel.baseline : subtitleLabel.top;
                topMargin: (text != "" && (subtitleLabel.text != "" || title.text != "")) ? units.gu(1) : 0
            }
            fontSize: "small"
            font.weight: Font.Light
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 2
        }
    }
}
