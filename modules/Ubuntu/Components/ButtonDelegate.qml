/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import Qt.labs.shaders 1.0


/*
  Style defined  in ButtonStyle
*/
Item {
    z: -1
    anchors.fill: parent

    // pick either a clear or dark text color depending on the luminance of the
    // background color to maintain good contrast (works in most cases)
    function __luminance(hexcolor){
        hexcolor = String(hexcolor)
        var r = parseInt(hexcolor.substr(1,2),16);
        var g = parseInt(hexcolor.substr(3,2),16);
        var b = parseInt(hexcolor.substr(5,2),16);
        return ((r*212)+(g*715)+(b*73))/1000/255;
    }

    Binding {
        target: widget
        property: "textColor"
        value: __luminance(base.color) <= 0.72 ? "white" : "#757373"
    }

    // FIXME: think of using distance fields
    BorderImage {
        id: shape

        anchors.fill: parent

        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
        source: widget.darkBorder ? Qt.resolvedUrl(widget.styleObject.shapeDark) : Qt.resolvedUrl(widget.styleObject.shapeNormal)
        border.left: 18; border.top: 15
        border.right: 18; border.bottom: 15
    }

    // FIXME: might become a paper texture
    Rectangle {
        id: base

        anchors.fill: shape
        color: widget.pressed ? widget.pressedColor : widget.color

    }

    ButtonMaskEffect {
        anchors.fill: shape
        gradientStrength: widget.pressed ? 0.0 : 1.0
        Behavior on gradientStrength {NumberAnimation {duration: 100; easing.type: Easing.OutQuad}}

        mask: ShaderEffectSource {sourceItem: shape; live: true; hideSource: true}
        base: ShaderEffectSource {sourceItem: base; live: true; hideSource: true}
    }

    // FIXME: could be generated from the shape (shadow parameters specified in guidelines)
    BorderImage {
        id: border

        anchors.fill: parent
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
        //source: widget.styleObject.borderImage
        source: (widget.darkBorder) ? (widget.pressed ? Qt.resolvedUrl(widget.styleObject.borderDarkPressed) : Qt.resolvedUrl(widget.styleObject.borderDarkIdle))
                            : (widget.pressed ? Qt.resolvedUrl(widget.styleObject.borderPressed) : Qt.resolvedUrl(widget.styleObject.borderIdle));
        border.left: 14; border.top: 17
        border.right: 15; border.bottom: 18
    }
}
