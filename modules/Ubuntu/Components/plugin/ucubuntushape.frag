// Static flow control (branching on a uniform value) is fast on most GPUs (including ultra-low
// power ones) because it allows to use the same shader execution path for an entire draw call. We
// rely on that technique here (also known as "uber-shader" solution) to avoid the complexity of
// dealing with a multiple shaders solution.

// FIXME(loicm)
//  - Check GPU behavior with regards to static flow control.
//  - Ensure binary flag testing doesn't prevent static flow control.
//  - Binary operator '&' is supported starting from GLSL 1.3 (OpenGL 3).

uniform sampler2D shapeTexture;
uniform sampler2D sourceTexture;
uniform lowp vec4 backgroundColor;
uniform lowp vec4 secondaryBackgroundColor;
uniform lowp vec4 overlayColor;
uniform mediump vec4 overlaySteps;
uniform lowp float sourceOpacity;
uniform lowp float opacity;
uniform lowp int flags;

varying mediump vec2 shapeCoord;
varying mediump vec2 quadCoord;
varying mediump vec2 sourceCoord;

const lowp int TEXTURED_FLAG = 0x1;
const lowp int OVERLAID_FLAG = 0x2;

void main(void)
{
    // Early texture fetch to cover latency as best as possible.
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);

    // Get the background color.
    lowp vec4 color = mix(backgroundColor, secondaryBackgroundColor, quadCoord.t);

    // Blend the source over the current color (static flow control prevents the texture fetch).
    if (flags & TEXTURED_FLAG) {
        lowp vec4 source = texture2D(sourceTexture, sourceCoord) * sourceOpacity;
        color = vec4(1.0 - source.a) * color + source;
    }

    // Get the overlay color and blend it over the current color.
    if (flags & OVERLAID_FLAG) {
        mediump vec4 steps = step(overlaySteps, quadCoord.stst);
        steps.xy = -steps.xy * steps.zw + steps.xy;
        lowp vec4 overlay = vec4(steps.x * steps.y) * overlayColor;
        color = vec4(1.0 - overlay.a) * color + overlay;
    }

    // Shape the current color with the mask.
    color *= vec4(shapeData.b);

    // Blend the border color over the current color.
    color = vec4(1.0 - shapeData.r) * color + shapeData.gggr;

    gl_FragColor = color * vec4(opacity);
}
