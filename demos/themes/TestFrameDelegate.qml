import QtQuick 2.0

Rectangle {
    z: -1
    anchors.fill: parent
    radius: 8
    border.color: (itemStyle && itemStyle.hasOwnProperty("border")) ? itemStyle.border : "brown"
    border.width: 4
    color: (itemStyle && itemStyle.hasOwnProperty("color")) ? itemStyle.color : "#f97897"
    smooth: true
}
