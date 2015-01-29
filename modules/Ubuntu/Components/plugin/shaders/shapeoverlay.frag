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

// Static flow control (branching on a uniform value) is fast on most GPUs (including ultra-low
// power ones) because it allows to use the same shader execution path for an entire draw call. We
// rely on that technique here (also known as "uber-shader" solution) to avoid the complexity of
// dealing with a multiple shaders solution.
// FIXME(loicm) Validate GPU behavior with regards to static flow control.

uniform sampler2D shapeTexture;
uniform sampler2D sourceTexture;
uniform lowp float sourceOpacity;
uniform lowp float opacity;
uniform bool textured;

varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp vec4 backgroundColor;
varying mediump vec2 overlayCoord;
varying lowp vec4 overlayColor;

void main(void)
{
    // Early texture fetch to cover latency as best as possible.
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);

    lowp vec4 color = backgroundColor;

    // FIXME(loicm) Would be better to use a bitfield but bitwise ops have only been integrated in
    //     GLSL 1.3 (OpenGL 3) and GLSL ES 3 (OpenGL ES 3).
    if (textured) {
        // Blend the source over the current color (static flow control prevents the texture fetch).
        lowp vec2 axisMask = -sign((sourceCoord.zw * sourceCoord.zw) - vec2(1.0));
        lowp float mask = clamp(axisMask.x + axisMask.y, 0.0, 1.0);
        lowp vec4 source = texture2D(sourceTexture, sourceCoord.st) * vec4(sourceOpacity * mask);
        color = vec4(1.0 - source.a) * color + source;
    }

    // Blend the overlay over the current color.
    lowp vec2 overlayAxisMask = -sign((overlayCoord * overlayCoord) - vec2(1.0));
    lowp float overlayMask = clamp(overlayAxisMask.x + overlayAxisMask.y, 0.0, 1.0);
    lowp vec4 overlay = overlayColor * vec4(overlayMask);
    color = vec4(1.0 - overlay.a) * color + overlay;

    // Shape the current color with the mask.
    color *= vec4(shapeData.b);

    // Blend the border over the current color.
    color = vec4(1.0 - shapeData.r) * color + shapeData.gggr;

    gl_FragColor = color * vec4(opacity);
}
