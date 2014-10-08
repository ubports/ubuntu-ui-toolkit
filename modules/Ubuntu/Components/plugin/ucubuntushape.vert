uniform lowp mat4 matrix;
attribute lowp vec4 positionAttrib;
attribute lowp vec2 shapeCoordAttrib;
attribute lowp vec2 imageCoordAttrib;
varying lowp vec2 shapeCoord;
varying lowp vec2 imageCoord;

void main()
{
    shapeCoord = shapeCoordAttrib;
    imageCoord = imageCoordAttrib;
    gl_Position = matrix * positionAttrib;
}
