uniform lowp mat4 matrix;
uniform lowp int flags;

attribute lowp vec4 positionAttrib;
attribute lowp vec2 shapeCoordAttrib;
attribute lowp vec4 sourceCoordAttrib;
attribute lowp vec4 backgroundColorAttrib;

// FIXME(loicm) Optimize by reducing/packing varyings.
varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp vec4 backgroundColor;

const lowp int TEXTURED_FLAG = 0x1;
const lowp int OVERLAID_FLAG = 0x2;

void main()
{
    shapeCoord = shapeCoordAttrib;
    backgroundColor = backgroundColorAttrib;

    if (flags & TEXTURED_FLAG) {
        sourceCoord = sourceCoordAttrib;
    }

    gl_Position = matrix * positionAttrib;
}
