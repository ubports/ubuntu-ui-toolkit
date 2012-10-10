import QtQuick 2.0

Rectangle {
    z: -1
    anchors.fill: parent
    radius: 8
    border.color: (widget.style.border != undefined) ? widget.style.border : "brown"
    border.width: 4
    color: widget.style.color
    smooth: true
}
