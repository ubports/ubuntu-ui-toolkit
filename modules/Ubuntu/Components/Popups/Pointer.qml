import QtQuick 2.0

ShaderEffect {
    property color color

    // Using Item.Rotation does not play well with the
    //  translation that would be needed after rotating.
    property real longAxis
    property real shortAxis

    // up, down, left or right, or none to hide the pointer
    property string direction: "down"

    width: rotate ? shortAxis : longAxis
    height: rotate ? longAxis : shortAxis

    // FIXME: It would be nicer to have a single transformation matrix that flips and rotates,
    //  but I did not manage to get a 3x3 matrix from QML into the shader.
    // Note: The properties declared below are internal, but cannot be prefixed with __ because
    //  that is not supported in the shaders where the same variable names are used.

    // rotate pointer 90 degrees
    property bool rotate: (direction === "left" || direction === "right")

    // flip the direction of the pointer
    property bool flip: (direction === "left" || direction === "up")

    visible: (direction !== "none")

    vertexShader: "
        uniform highp mat4 qt_Matrix;
        attribute highp vec4 qt_Vertex;
        attribute highp vec2 qt_MultiTexCoord0;

        uniform bool flip;
        uniform bool rotate;
        varying highp vec2 coord;

        void main() {
            if (rotate) coord = qt_MultiTexCoord0.ts;
            else coord = qt_MultiTexCoord0.st;

            if (flip) coord = vec2(coord.x, 1.0 - coord.y);

            gl_Position = qt_Matrix * qt_Vertex;
        }"

    fragmentShader: "
        varying highp vec2 coord;
        uniform vec4 color;
        uniform float opacity;

        void main(void) {
            if (coord.t > 2.0*(1.0 - coord.s)) discard;
            if (coord.t > 2.0*coord.s) discard;
            gl_FragColor = color * vec4(opacity);
        }"
}
