/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

ShaderEffect {
    property variant source
    property real targetWidth: 0
    property real targetHeight: 0

    property real radius: 50.0
    property real diffractionIndex: 0.5

    // Transform slider values, and bind result to shader uniforms
    property real posX: targetWidth / 2
    property real posY: targetHeight / 2

    fragmentShader: d.fragmentShaderCommon
    QtObject {
        id: d
        property string fragmentShaderCommon: "
            #ifdef GL_ES
                precision mediump float;
            #else
            #   define lowp
            #   define mediump
            #   define highp
            #endif // GL_ES
            // Based on http://www.reddit.com/r/programming/comments/losip/shader_toy/c2upn1e

            uniform sampler2D source;
            uniform lowp float qt_Opacity;
            varying vec2 qt_TexCoord0;
            uniform float radius;
            uniform float diffractionIndex;
            uniform float targetWidth;
            uniform float targetHeight;
            uniform float posX;
            uniform float posY;

            void main()
            {
                vec2 tc = qt_TexCoord0;
                vec2 center = vec2(posX, posY);
                vec2 xy = gl_FragCoord.xy - center.xy;
                float r = sqrt(xy.x * xy.x + xy.y * xy.y);
                if (r < radius) {
                    float h = diffractionIndex * 0.5 * radius;
                    vec2 new_xy = r < radius ? xy * (radius - h) / sqrt(radius * radius - r * r) : xy;
                    vec2 targetSize = vec2(targetWidth, targetHeight);
                    tc = (new_xy + center) / targetSize;
                    tc.y = 1.0 - tc.y;
                }
                gl_FragColor = qt_Opacity * texture2D(source, tc);
            }
        "
    }
}
