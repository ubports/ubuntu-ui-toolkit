// Copyright © 2015 Canonical Ltd.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Loïc Molinari <loic.molinari@canonical.com>

uniform highp mat4 matrix;  // mediump was interpreted as lowp on PowerVR Rogue G6200 (arale).
uniform bool textured;

attribute highp vec4 positionAttrib;  // highp because of matrix precision qualifier.
attribute mediump vec2 shapeCoordAttrib;
attribute mediump vec4 sourceCoordAttrib;
attribute lowp float yCoordAttrib;
attribute lowp vec4 backgroundColorAttrib;
attribute mediump vec2 overlayCoordAttrib;
attribute lowp vec4 overlayColorAttrib;

// FIXME(loicm) Optimize by reducing/packing varyings.
varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp float yCoord;
varying lowp vec4 backgroundColor;
varying mediump vec2 overlayCoord;
varying lowp vec4 overlayColor;

void main()
{
    shapeCoord = shapeCoordAttrib;
    if (textured) {
        sourceCoord = sourceCoordAttrib;
    }
    yCoord = yCoordAttrib;
    backgroundColor = backgroundColorAttrib;
    overlayCoord = overlayCoordAttrib;
    overlayColor = overlayColorAttrib;

    gl_Position = matrix * positionAttrib;
}
