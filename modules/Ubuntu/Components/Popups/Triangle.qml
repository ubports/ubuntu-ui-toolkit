import QtQuick 2.0

    ShaderEffect {
        property color color

        fragmentShader:
            "
            varying highp vec2 qt_TexCoord0;
            uniform vec4 color;
            uniform float opacity;

            void main(void) {

                if (qt_TexCoord0.t > 2.0*(1.0 - qt_TexCoord0.s)) discard;
                if (qt_TexCoord0.t > 2.0*qt_TexCoord0.s) discard;


//                gl_FragColor = vec4(qt_TexCoord0.s, qt_TexCoord0.t, 0.0, 1.0);
                gl_FragColor = color * vec4(opacity);
            }
            "
    }
//}
