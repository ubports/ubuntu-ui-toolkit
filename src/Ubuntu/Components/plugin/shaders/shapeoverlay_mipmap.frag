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
uniform lowp vec2 opacityFactors;
uniform lowp float dfdtFactor;
uniform lowp float sourceOpacity;
uniform lowp float distanceAA;
uniform bool textured;
uniform mediump int aspect;

varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp float yCoord;
varying lowp vec4 backgroundColor;
varying mediump vec2 overlayCoord;
varying lowp vec4 overlayColor;

const mediump int FLAT        = 0x08;  // 1 << 3
const mediump int INSET       = 0x10;  // 1 << 4
const mediump int DROP_SHADOW = 0x20;  // 1 << 5

void main(void)
{
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);
    lowp vec4 color = backgroundColor;

    if (textured) {
        // Blend the source over the current color.
        // FIXME(loicm) sign() is far from optimal. Call texture2D() at beginning of scope.
        lowp vec2 axisMask = -sign((sourceCoord.zw * sourceCoord.zw) - vec2(1.0));
        lowp float mask = clamp(axisMask.x + axisMask.y, 0.0, 1.0);
        lowp vec4 source = texture2D(sourceTexture, sourceCoord.st) * vec4(sourceOpacity * mask);
        color = vec4(1.0 - source.a) * color + source;
    }

    // Blend the overlay over the current color.
    // FIXME(loicm) sign() is far from optimal.
    lowp vec2 overlayAxisMask = -sign((overlayCoord * overlayCoord) - vec2(1.0));
    lowp float overlayMask = clamp(overlayAxisMask.x + overlayAxisMask.y, 0.0, 1.0);
    lowp vec4 overlay = overlayColor * vec4(overlayMask);
    color = vec4(1.0 - overlay.a) * color + overlay;

    if (aspect == FLAT) {
        // Mask the current color.
        color *= shapeData.b;

    } else if (aspect == INSET) {
        // The vertex layout of the shape is made so that the derivative is negative from top to
        // middle and positive from middle to bottom.
        lowp float shapeSide = yCoord <= 0.0 ? 0.0 : 1.0;
        // Blend the shape inner shadow over the current color. The shadow color is black, its
        // translucency is stored in the texture.
        lowp float shadow = shapeData[int(shapeSide)];
        color = vec4(1.0 - shadow) * color + vec4(0.0, 0.0, 0.0, shadow);
        // Get the shape mask.
        lowp vec2 mask = shapeData.ba;
        // Get the bevel color. The bevel is made of the top mask masked with the bottom mask. A
        // gradient from the bottom (1) to the middle (0) of the shape is used to factor out values
        // resulting from the mask anti-aliasing. The bevel color is white with 60% opacity.
        lowp float bevel = (mask.x * -mask.y) + mask.x;  // -ab + a = a(1 - b)
        lowp float gradient = clamp((shapeSide * -shapeCoord.t) + shapeSide, 0.0, 1.0);
        bevel *= gradient * 0.6;
        // Mask the current color then blend the bevel over the resulting color. We simply use
        // additive blending since the bevel has already been masked.
        color = (color * vec4(mask[int(shapeSide)])) + vec4(bevel);

    } else if (aspect == DROP_SHADOW) {
        // Get the shape mask.
        lowp int shapeSide = yCoord <= 0.0 ? 0 : 1;
        lowp float mask = shapeData[shapeSide];
        // Get the shadow color outside of the shape mask.
        lowp float shadow = (shapeData.b * -mask) + shapeData.b;  // -ab + a = a(1 - b)
        // Mask the current color then blend the shadow over the resulting color. We simply use
        // additive blending since the shadow has already been masked.
        color = (color * vec4(mask)) + vec4(0.0, 0.0, 0.0, shadow);
    }

    gl_FragColor = color * opacityFactors.xxxy;
}
