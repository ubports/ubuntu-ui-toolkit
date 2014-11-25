// Copyright © 2014 Canonical Ltd.
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

uniform mediump mat4 matrix;
uniform lowp int flags;

attribute mediump vec4 positionAttrib;
attribute mediump vec2 shapeCoordAttrib;
attribute mediump vec4 sourceCoordAttrib;
attribute lowp vec4 backgroundColorAttrib;

// FIXME(loicm) Optimize by reducing/packing varyings.
varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp vec4 backgroundColor;

const lowp int TEXTURED_FLAG = 0x1;

void main()
{
    shapeCoord = shapeCoordAttrib;
    backgroundColor = backgroundColorAttrib;

    if (flags & TEXTURED_FLAG) {
        sourceCoord = sourceCoordAttrib;
    }

    gl_Position = matrix * positionAttrib;
}
