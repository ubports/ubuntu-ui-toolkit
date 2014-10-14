// Static flow control (branching on a uniform value) is fast on most GPUs (including ultra-low
// power ones) because it allows to use the same shader execution path for an entire draw call. We
// rely on that technique here (also known as "uber-shader" solution) to avoid the complexity of
// dealing with a multiple shaders solution.

// FIXME(loicm)
//  - Check GPU behavior with regards to static flow control.
//  - Ensure binary flag testing doesn't prevent static flow control.

uniform lowp float opacity;
uniform sampler2D shapeTexture;
uniform sampler2D imageTexture;
uniform lowp vec4 color1;
uniform lowp vec4 color2;
uniform lowp vec4 overlayColor;
uniform mediump vec4 overlaySteps;
uniform lowp int flags;

varying mediump vec2 shapeCoord;
varying mediump vec2 quadCoord;
varying mediump vec2 imageCoord;

const lowp int COLORED_FLAG  = 0x1;
const lowp int OVERLAID_FLAG = 0x2;

void main(void)
{
    lowp vec4 color;

    // Early texture fetch to cover latency as best as possible.
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);

    // Get the background color (static flow control prevents evaluating the texture fetch in case
    // of a colored shape).
    if (flags & COLORED_FLAG) {
        color = mix(color1, color2, quadCoord.t);
    } else {
        color = texture2D(imageTexture, imageCoord);
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
