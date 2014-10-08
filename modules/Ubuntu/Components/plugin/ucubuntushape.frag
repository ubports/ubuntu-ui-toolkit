// Static flow control (branching on a uniform value) is fast on most GPUs (including ultra-low
// power ones) because it allows to use the same shader execution path for an entire draw call. We
// rely on that technique here (also known as "uber-shader" solution) to avoid the complexity of
// dealing with a multiple shaders solution.

uniform lowp float opacity;
uniform sampler2D shapeTexture;
uniform sampler2D imageTexture;
uniform lowp vec4 color1;
uniform lowp vec4 color2;
uniform lowp bool colored;
varying lowp vec2 shapeCoord;
varying lowp vec2 imageCoord;

void main(void)
{
    // Early texture fetch to cover latency as best as possible.
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);

    // Get the shaped color. Note that static flow control prevents evaluating the texture fetch
    // in case of a colored shape.
    lowp vec4 color;
    if (colored) {
        color = mix(color1, color2, imageCoord.t) * vec4(shapeData.b);
    } else {
        color = texture2D(imageTexture, imageCoord) * vec4(shapeData.b);
    }

    // Standard Porter/Duff source over blending.
    lowp vec4 blend = shapeData.gggr + vec4(1.0 - shapeData.r) * color;
    gl_FragColor = blend * vec4(opacity);
}
