uniform lowp mat4 matrix;
uniform lowp vec4 atlasTransform;
uniform lowp int flags;

attribute lowp vec4 positionAttrib;
attribute lowp vec2 shapeCoordAttrib;
attribute lowp vec2 quadCoordAttrib;

// FIXME(loicm) Optimize by reducing/packing varyings.
varying mediump vec2 shapeCoord;
varying mediump vec2 quadCoord;
varying mediump vec2 imageCoord;

const lowp int COLORED_FLAG  = 0x1;
const lowp int OVERLAID_FLAG = 0x2;

void main()
{
    shapeCoord = shapeCoordAttrib;

    if (!(flags & COLORED_FLAG)) {
        imageCoord = quadCoordAttrib * atlasTransform.xy + atlasTransform.zw;
    }

    if (flags & (COLORED_FLAG | OVERLAID_FLAG)) {
        quadCoord = quadCoordAttrib;
    }

    gl_Position = matrix * positionAttrib;
}
