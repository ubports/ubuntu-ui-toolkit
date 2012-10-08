import QtQuick 2.0

Rectangle {
    z: -1
    anchors.fill: parent
    radius: 8
    border.color: (widget.styleObject.border != undefined) ? widget.styleObject.border : "brown"
    border.width: 4
    color: widget.styleObject.color
    smooth: true
}
