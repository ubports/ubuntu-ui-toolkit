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
