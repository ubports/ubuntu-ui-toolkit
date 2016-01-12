/*
 * Copyright 2016 Canonical Ltd.
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
 *
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

uniform sampler2D texture;
uniform lowp float opacity;
varying mediump vec2 texCoord1;
varying mediump vec2 texCoord2;
varying lowp vec4 color;

void main(void)
{
    lowp float shapeOut = texture2D(texture, texCoord1).r;
    lowp float shapeIn = texture2D(texture, texCoord2).r;
    // FMA'd shapeOut * (1.0 - shapeIn)
    lowp float shape = (shapeOut * -shapeIn) + shapeOut;
    // sin approx to make thinner corners (particularly visible at stroke 1).
    shape = (shape + 1.0) * (shape - 1.0) + 1.0;
    gl_FragColor = vec4(shape * opacity) * color;
}
