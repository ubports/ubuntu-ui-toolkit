uniform lowp mat4 matrix;

attribute lowp vec4 positionAttrib;
attribute lowp vec2 shapeCoordAttrib;
attribute lowp vec2 quadCoordAttrib;

varying mediump vec2 shapeCoord;
varying mediump vec2 quadCoord;

void main()
{
    shapeCoord = shapeCoordAttrib;
    quadCoord = quadCoordAttrib;
    gl_Position = matrix * positionAttrib;
}
