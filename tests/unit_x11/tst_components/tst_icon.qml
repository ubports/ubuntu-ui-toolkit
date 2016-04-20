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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

Item {
    width: units.gu(50)
    height: units.gu(50)

    Row {
        anchors.centerIn: parent
        height: units.gu(10)
        width: childrenRect.width

        Icon {
            // Fails to load the icon when suru-icon-theme or libqt5svg5 are
            // not installed. This causes a warning and rejection
            // by jenkins continuous integration.
            name: "add"
            height: parent.height
            width: height
        }
        Icon {
            // Fails when the icon is becoming invisible when non-atomic updates
            // cause sourceSize.width or sourceSize.height to be 0 before other
            // properties are updated.
            id: icon
            width: visible ? units.gu(10) : 0
            height: width
            name: "search"
        }
        Icon {
            id: icon2
            width: units.gu(10)
            height: width
        }
        Icon {
            // Regression test for bug #1371509
            // Will print "QML QQuickImage: Failed to get image from provider" when broken.
            id: icon3
            width: units.gu(10)
            height: width
            source: Qt.resolvedUrl("tst_icon-select.png")
        }
    }

    UbuntuTestCase {
        name: "Icon"
        when: windowShown

        SignalSpy {
            id: shaderSpy
            signalName: 'onStatusChanged'
        }

        function cleanup() {
            icon2.name = "";
        }

        function test_updateIconSize_bug1349769() {
            icon.visible = false;
            // causes "QML Image: Failed to get image from provider: image://theme/search"
            // warning when sourceSize.width or sourceSize.height becomes 0 while
            // while still trying to render the icon. Tests will pass with the warning, but
            // the MR is rejected by jenkins continuous integration.
        }

        function test_name() {
            icon2.name = "search";

            var image = findChild(icon2, "image");
            compare(image.source, "image://theme/search",
                    "Source of the image should be image://theme/{name}.");
        }

        function test_source() {
            icon2.name = "search";
            icon2.source = "/usr/share/icons/suru/actions/scalable/edit-find.svg";

            var image = findChild(icon2, "image");
            compare(image.source,
                    "file:///usr/share/icons/suru/actions/scalable/edit-find.svg",
                    "Source of the image should equal icon2.source.");
        }

        function test_keyColor() {
            icon.visible = true;
            var image = findChild(icon, "image");
            var shader = findChild(icon, "shader");
            shaderSpy.target = shader;

            compare(icon.name, 'search');
            compare(shader.visible, false);
            compare(shader.status, ShaderEffect.Uncompiled)
            icon.color = UbuntuColors.orange;
            shaderSpy.wait();
            compare(shader.log, '')
            // https://bugreports.qt.io/browse/QTBUG-49713
            // status may be Error with no log even if successful
            verify(shader.status !== ShaderEffect.Uncompiled)
            compare(shader.keyColorOut, icon.color);
            compare(shader.visible, true);
            compare(shader.source, image);
            icon.keyColor = UbuntuColors.purple;
            compare(shader.keyColorIn, icon.keyColor);
            // Unsetting the icon name should disable the shader
            icon.name = '';
            compare(icon.source, '');
            compare(shader.visible, false);
            // Let's get back to a valid source
            icon.name = 'search';
            compare(shader.visible, true);
            compare(shader.source, image);
            // Unsetting the keyColor should also disable the shader
            icon.color = Qt.rgba(0.0, 0.0, 0.0, 0.0);
            compare(shader.visible, false);
        }
    }
}
