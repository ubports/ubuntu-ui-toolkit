/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.0
import Qt.labs.shaders 1.0

ShaderEffectItem {
    property variant mask
    property variant base
    property variant gradient
    property real gradientStrength: 1.0

    fragmentShader:
        "
        varying highp vec2 qt_TexCoord0;
        uniform lowp float qt_Opacity;
        uniform sampler2D mask;
        uniform sampler2D base;
        uniform sampler2D gradient;
        uniform lowp float gradientStrength;

        vec3 blend(vec3 baseVec, vec3 blendVec)
        {
            vec3 e = vec3(1.0);
            vec3 g = step(vec3(0.5), baseVec);

            // a: baseVec[i]
            // b: blendVec[i]
            // a < 0.5 ==> g = 0
            // a > 0.5 ==> g = 1

            vec3 m = mix(g*(e-baseVec), (e-g)*baseVec, blendVec);
            // a < 0.5 ==> m[i] = a*b
            // a > 0.5 ==> m[i] = (1-a)*(1-b)
            return mix(g, e-g, vec3(2.0)*m);
            // a < 0.5 ==> return[i]:   2*m[i] = 2*a*b
            // a > 0.5 ==> return[i]: 1-2*m[i] = 1-2*(1-a)*(1-b)
        }

        void main(void)
        {
            lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);
            if (maskColor.a == 0.0) discard;

            lowp vec4 baseColor = texture2D(base, qt_TexCoord0.st);
            lowp vec4 gradientColor = texture2D(gradient, qt_TexCoord0.st);
            lowp vec4 result = vec4(blend(baseColor.rgb, gradientColor.rgb), baseColor.a);
            gl_FragColor = mix(baseColor, result, gradientStrength) * maskColor.a;
        }
        "
}
