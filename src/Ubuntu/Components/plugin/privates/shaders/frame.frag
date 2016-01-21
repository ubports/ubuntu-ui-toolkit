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
varying mediump vec2 outerCoord;
varying mediump vec2 innerCoord;
varying lowp vec4 color;

void main(void)
{
    lowp float shapeOut = texture2D(texture, outerCoord).r;
    lowp float shapeIn = texture2D(texture, innerCoord).r;
    // Fused multiply-add friendly version of (shapeOut * (1.0 - shapeIn))
    lowp float shape = (shapeOut * -shapeIn) + shapeOut;
    // shape is squared to make thinner corners (particularly visible at stroke 1).
    gl_FragColor = vec4(shape * shape * opacity) * color;
}
