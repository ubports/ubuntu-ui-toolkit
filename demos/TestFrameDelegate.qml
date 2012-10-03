import QtQuick 1.1

Rectangle {
    z: -1
    anchors.fill: parent
    radius: 8
    border.color: (widget.styleObject.border != undefined) ? widget.styleObject.border : "brown"
    border.width: 4
    //color: widget.pressed ? widget.pressedColor : widget.color
    color: widget.styleObject.color
    smooth: true
}
